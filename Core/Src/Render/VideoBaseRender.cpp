#include "VideoBaseRender.h"
#include "platform\PlatformMethods.h"
#include "EGL\eglext.h"
#include "GLES2\gl2.h"
#include "../../Log/Interface/EZLogInterface.h"
#include <vector>

VideoBaseRender::VideoBaseRender(long windid, AVPixelFormat format) {
    m_windid = windid;
    m_renderFormat = format;
    kVS = R"(attribute vec4 vPosition;
void main()
{
    gl_Position = vPosition;
})";


    kFS = R"(precision mediump float;
void main()
{
    gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
})";
}

VideoBaseRender::~VideoBaseRender() {
    glDeleteProgram(m_programId);
    glDeleteShader(m_vertShader);
    glDeleteShader(m_fragShader);
    destroyGL();
}

bool VideoBaseRender::init() {
    // init display
    auto native_display = m_windid != 0 ? GetDC((HWND)m_windid) : EGL_DEFAULT_DISPLAY;
    std::vector<EGLAttrib> displayAttributes;
    displayAttributes.push_back(EGL_PLATFORM_ANGLE_TYPE_ANGLE);
    displayAttributes.push_back(EGL_PLATFORM_ANGLE_TYPE_D3D11_ANGLE);
    displayAttributes.push_back(EGL_PLATFORM_ANGLE_MAX_VERSION_MAJOR_ANGLE);
    displayAttributes.push_back(EGL_DONT_CARE);
    displayAttributes.push_back(EGL_PLATFORM_ANGLE_MAX_VERSION_MINOR_ANGLE);
    displayAttributes.push_back(EGL_DONT_CARE);
    displayAttributes.push_back(EGL_PLATFORM_ANGLE_DEVICE_TYPE_ANGLE);
    displayAttributes.push_back(EGL_PLATFORM_ANGLE_DEVICE_TYPE_HARDWARE_ANGLE); // …Ë÷√‰÷»æ…Ë±∏
    angle::PlatformMethods* methods = nullptr;
    if (ANGLEGetDisplayPlatform(nullptr, angle::g_PlatformMethodNames, angle::g_NumPlatformMethods, nullptr, &methods)) {
        displayAttributes.push_back(EGL_PLATFORM_ANGLE_PLATFORM_METHODS_ANGLEX);
        displayAttributes.push_back(reinterpret_cast<EGLAttrib>(methods));
    }
    displayAttributes.push_back(EGL_NONE);
    m_display = eglGetPlatformDisplay(EGL_PLATFORM_ANGLE_ANGLE, native_display, displayAttributes.data());
    if (m_display == EGL_NO_DISPLAY){
        auto error = eglGetError();
        destroyGL();
        return false;
    }
    EGLint major, minor;
    if (eglInitialize(m_display, &major, &minor) == EGL_FALSE){
        destroyGL();
        return false;
    }

    auto renderType = EGL_OPENGL_ES2_BIT;
    if (minor >= 4) {
        renderType = EGL_OPENGL_ES3_BIT_KHR;
    }
    std::vector<EGLint> configAttributes = {
        EGL_SURFACE_TYPE,
        EGL_WINDOW_BIT,
        EGL_RED_SIZE,8,
        EGL_GREEN_SIZE,8,
        EGL_BLUE_SIZE,8,
        EGL_ALPHA_SIZE,8,
        EGL_DEPTH_SIZE,24,
        EGL_STENCIL_SIZE,8,
        EGL_SAMPLE_BUFFERS,0,
        EGL_RENDERABLE_TYPE, renderType,
        EGL_NONE
    };
    GLint numConfigs = 0;
    eglChooseConfig(m_display, configAttributes.data(), &m_config, 1, &numConfigs);


    // init context
    std::vector<EGLint> contextAttributes = {
        EGL_CONTEXT_CLIENT_VERSION, 3,
        EGL_NONE
    };
    m_context = eglCreateContext(m_display, m_config, nullptr, contextAttributes.data());

    // init surface
    const char* displayExtensions = eglQueryString(m_display, EGL_EXTENSIONS);
    std::vector<EGLint> surfaceAttributes;
    if (strstr(displayExtensions, "EGL_NV_post_sub_buffer") != nullptr){
        surfaceAttributes.push_back(EGL_POST_SUB_BUFFER_SUPPORTED_NV);
        surfaceAttributes.push_back(EGL_TRUE);
    }
    surfaceAttributes.push_back(EGL_NONE);
    m_surface = eglCreateWindowSurface(m_display, m_config, (HWND)m_windid, surfaceAttributes.data());

    // parse device
    auto queryDisplayAttribEXT = (PFNEGLQUERYDISPLAYATTRIBEXTPROC)eglGetProcAddress("eglQueryDisplayAttribEXT");
    auto queryDeviceAttribEXT = (PFNEGLQUERYDEVICEATTRIBEXTPROC)eglGetProcAddress("eglQueryDeviceAttribEXT");
    if (queryDisplayAttribEXT && queryDeviceAttribEXT) {
        EGLAttrib angleDevice(0), d3d11Device{ 0 }, d3d9Device{ 0 };
        if (queryDisplayAttribEXT(m_display, EGL_DEVICE_EXT, &angleDevice) == EGL_TRUE && angleDevice) {
            if (queryDeviceAttribEXT(reinterpret_cast<EGLDeviceEXT>(angleDevice), EGL_D3D11_DEVICE_ANGLE, &d3d11Device) == EGL_TRUE) {
                m_glD3d11Device = reinterpret_cast<ID3D11Device*>(d3d11Device);
            }
            else if (queryDeviceAttribEXT(reinterpret_cast<EGLDeviceEXT>(angleDevice), EGL_D3D9_DEVICE_ANGLE, &d3d9Device) == EGL_TRUE) {
                m_glD3d9Device = reinterpret_cast<IDirect3DDevice9*>(d3d9Device);
            }
        }
    }

    // use current contex
    eglMakeCurrent(m_display, m_surface, m_surface, m_context);
}

void VideoBaseRender::render() {
    if (m_programId == 0) {
        m_programId = glCreateProgram();
        if (!compileSharder(&m_vertShader, GL_VERTEX_SHADER, kVS.c_str()) || !compileSharder(&m_fragShader, GL_FRAGMENT_SHADER, kFS.c_str())) {
            return;
        }
        glAttachShader(m_programId, m_vertShader);
        glAttachShader(m_programId, m_fragShader);
        glLinkProgram(m_programId);
    }

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GLfloat vertices[] = {
            0.0f, 0.5f, 0.0f, -0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f,
    };

    RECT RC;
    ::GetClientRect((HWND)m_windid, &RC);
    // Set the viewport
    glViewport(0, 0, RC.right - RC.left, RC.bottom - RC.top);

    // Use the program object
    glUseProgram(m_programId);

    // Load the vertex data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vertices);
    glEnableVertexAttribArray(0);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    eglSwapBuffers(m_display, m_surface);
}

bool VideoBaseRender::compileSharder(GLuint* shader, GLenum shaderType, const char* source) {
    *shader = glCreateShader(shaderType);

    const char* sourceArray[1] = { source };
    glShaderSource(*shader, 1, sourceArray, nullptr);
    glCompileShader(*shader);

    GLint compileResult;
    glGetShaderiv(*shader, GL_COMPILE_STATUS, &compileResult);

    if (compileResult == 0){
        GLint infoLogLength;
        glGetShaderiv(*shader, GL_INFO_LOG_LENGTH, &infoLogLength);
        if (infoLogLength > 1){
            std::vector<GLchar> infoLog(infoLogLength);
            glGetShaderInfoLog(*shader, static_cast<GLsizei>(infoLog.size()), nullptr, &infoLog[0]);
        }
        glDeleteShader(*shader);
        *shader = 0;
        return false;
    }

    return true;
}

GLint VideoBaseRender::createTextureFromImgaeData(void* imageData, int width, int height) {
    GLuint texture_ID = 0;
    glGenTextures(1, &texture_ID);
    if (texture_ID == 0){
        return 0;
    }
    GLint lastTextureID = 0;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &lastTextureID);
    glBindTexture(GL_TEXTURE_2D, texture_ID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
    glBindTexture(GL_TEXTURE_2D, lastTextureID);
    return texture_ID;
}

void VideoBaseRender::destroyGL()
{
    destroyContext();
    destroySurface();

    if (m_display != EGL_NO_DISPLAY){
        eglMakeCurrent(m_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        eglTerminate(m_display);
        m_display = EGL_NO_DISPLAY;
    }
}

void VideoBaseRender::destroySurface()
{
    if (m_surface != EGL_NO_SURFACE){
        eglMakeCurrent(m_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        eglDestroySurface(m_display, m_surface);
        m_surface = EGL_NO_SURFACE;
    }
}

void VideoBaseRender::destroyContext()
{
    if (m_context != EGL_NO_CONTEXT){
        eglMakeCurrent(m_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        eglDestroyContext(m_display, m_context);
        m_context = EGL_NO_CONTEXT;
    }
}