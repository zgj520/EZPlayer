#include "YUV420PToRGBARender.h"
#include "d3d11.h"
#include "EGL\eglext.h"
#include <wrl/client.h>

YUV420PToRGBARender::YUV420PToRGBARender(long windid):VideoBaseRender(windid) {
    kVS = R"(
attribute vec4 vPosition;
attribute vec2 aSamplerCoord;
varying vec2 vSamplerCoord;
void main()
{
    vSamplerCoord = aSamplerCoord;
    gl_Position = vPosition;
})";

    kFS = R"(
precision mediump float;
varying vec2 vSamplerCoord;
uniform sampler2D uSamplerTexture1;
uniform sampler2D uSamplerTexture2;
uniform sampler2D uSamplerTexture3;
void main()
{
    vec3 yuv;
    yuv.x = texture2D(uSamplerTexture1, vSamplerCoord).r - 16.0/235.0;
    yuv.y = texture2D(uSamplerTexture2, vSamplerCoord).r - 128.0/240.0;
    yuv.z = texture2D(uSamplerTexture3, vSamplerCoord).r - 128.0/240.0;
    mat3 yuv_to_rgb = mat3(1.164, 1.164,  1.164,
                           0.0,    -0.213, 2.112,
                           1.793, -0.533, 0.0);
    gl_FragColor = vec4(yuv_to_rgb * yuv, 1.0);
})";
}

GLuint YUV420PToRGBARender::genTexture(int width, int height) {
    GLuint texture_ID = 0;
    glGenTextures(1, &texture_ID);
    GLint lastTextureID = 0;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &lastTextureID);
    glBindTexture(GL_TEXTURE_2D, texture_ID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, lastTextureID);
    return texture_ID;
}

void YUV420PToRGBARender::render(AVFrame* frame) {
    if (m_programId == 0) {
        m_programId = glCreateProgram();
        if (!compileSharder(&m_vertShader, GL_VERTEX_SHADER, kVS.c_str()) || !compileSharder(&m_fragShader, GL_FRAGMENT_SHADER, kFS.c_str())) {
            m_programId = 0;
            return;
        }
        glAttachShader(m_programId, m_vertShader);
        glAttachShader(m_programId, m_fragShader);
        glLinkProgram(m_programId);
    }

    int width = frame->width;
    int height = frame->height;

    RECT RC;
    ::GetClientRect((HWND)m_windid, &RC);
    // Set the viewport
    glViewport(0, 0, RC.right - RC.left, RC.bottom - RC.top);

    // Use the program object
    glUseProgram(m_programId);

    glClearColor(1.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //定义定点数组
    float vertex_coord_data[] = {
            -1.f, -1.f, 0.f,   0.f, 1.f,
            -1.f,  1.f, 0.f,   0.f, 0.f,
             1.f,  1.f, 0.f,   1.f, 0.f,
             1.f, -1.f, 0.f,   1.f, 1.f,
    };

    uint32_t vertx_index_data[] = {
        0, 1, 2,
        2, 3, 0
    };

    uint32_t m_vertex_buffer, m_index_buffer;
    glGenBuffers(1, &m_vertex_buffer);
    glGenBuffers(1, &m_index_buffer);

    GLuint m_vertex_array;
    glGenVertexArrays(1, &m_vertex_array);
    glBindVertexArray(m_vertex_array);

    glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_coord_data), vertex_coord_data, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertx_index_data), vertx_index_data, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

    if (m_yTextureId == 0) {
        m_yTextureId = genTexture(width, height);
    }
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_yTextureId);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RED, GL_UNSIGNED_BYTE, frame->data[0]);
    glUniform1i(glGetUniformLocation(m_programId,"uSamplerTexture1"), 0);

    if (m_uTextureId == 0) {
        m_uTextureId = genTexture(width/2, height/2);
    }
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_uTextureId);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width/2, height/2, GL_RED, GL_UNSIGNED_BYTE, frame->data[1]);
    glUniform1i(glGetUniformLocation(m_programId, "uSamplerTexture2"), 1);

    if (m_vTextureId == 0) {
        m_vTextureId = genTexture(width / 2, height / 2);
    }
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_vTextureId);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width/2, height/2, GL_RED, GL_UNSIGNED_BYTE, frame->data[2]);
    glUniform1i(glGetUniformLocation(m_programId, "uSamplerTexture3"), 2);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, vertx_index_data);

    eglSwapBuffers(m_display, m_surface);

    glDeleteBuffers(1, &m_vertex_buffer);
    glDeleteBuffers(1, &m_index_buffer);
}
