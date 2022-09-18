#include "BMPImageRender.h"

BMPImageRender::BMPImageRender(long windid, const std::string& filePath)
    :VideoBaseRender(windid),
    m_filePath(filePath){
}
void BMPImageRender::render() {
    int width = 0, height = 0;
    GLubyte* pixelData = parseBmpImageData(width, height);
    if (pixelData == nullptr || width == 0 || height == 0) {
        return;
    }
    renderImage(pixelData, width, height);
    free(pixelData);
}

void BMPImageRender::renderImage(void* imageData, int width, int height) {
    if (m_programId == 0) {
        constexpr char kVS[] = R"(attribute vec4 position;
attribute vec2 textCoordinate;
varying lowp vec2 varyTextCoord;
void main() {
    varyTextCoord = textCoordinate;
    gl_Position = position;
})";

        constexpr char kFS[] = R"(precision mediump float;
varying lowp vec2 varyTextCoord;
uniform sampler2D colorMap;
void main() {
    vec3 rgb;
    rgb.r =  texture2D(colorMap, varyTextCoord).z;
    rgb.g =  texture2D(colorMap, varyTextCoord).y;
    rgb.b =  texture2D(colorMap, varyTextCoord).x;
    gl_FragColor = vec4(rgb, 1.0);
})";
        m_programId = glCreateProgram();
        if (!compileSharder(&m_vertShader, GL_VERTEX_SHADER, kVS) || !compileSharder(&m_fragShader, GL_FRAGMENT_SHADER, kFS)) {
            return;
        }
        glAttachShader(m_programId, m_vertShader);
        glAttachShader(m_programId, m_fragShader);
        glLinkProgram(m_programId);
    }

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    RECT RC;
    ::GetClientRect((HWND)m_windid, &RC);
    // Set the viewport
    glViewport(0, 0, RC.right - RC.left, RC.bottom - RC.top);

    // Use the program object
    glUseProgram(m_programId);

    //定义定点数组
    static const GLfloat quad_data[] =
    {
        -1.0f, -1.0f,
        1.0f, -1.0f,
        -1.0f, 1.0f,
        1.0f, 1.0f,

        0.0f, 0.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,
    };

    GLuint quad_vbo;
    glGenBuffers(1, &quad_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_data), quad_data, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)(8 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

    auto textureId = createTextureFromImgaeData(imageData, width, height);
    glActiveTexture(GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_2D, textureId);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    eglSwapBuffers(m_display, m_surface);
}


GLubyte* BMPImageRender::parseBmpImageData(int &width, int &height)
{
    FILE* pFile = fopen(m_filePath.c_str(), "rb");
    if (pFile == 0) {
        return nullptr;
    }
    BITMAPFILEHEADER bf;                      //图像文件头
    BITMAPINFOHEADER bi;                      //图像文件头信息
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, pFile);
    if (bf.bfType != 0x4D42){
        return nullptr;
    }
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, pFile);
    width = bi.biWidth;
    height = bi.biHeight;
    if (bi.biBitCount != 24){
        return nullptr;
    }

    // 原始数据是BGR格式，需要拼装一个A组成BGRA给GL
    auto buf = (GLubyte*)malloc(width * height * 4);
    fseek(pFile, long(sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)), 0);
    int index = 0;
    for (int i = 0; i < width * height;i++) {
        auto bufTemp = (GLubyte*)malloc(4);
        fread(bufTemp, 1, 3, pFile);
        bufTemp[3] = '0';
        memcpy(buf+ index, bufTemp, 4);
        free(bufTemp);
        index = index + 4;
    }
    return buf;
}