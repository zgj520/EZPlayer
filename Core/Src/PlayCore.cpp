#include "PlayCore.h"
#include "Render\BMPImageRender.h"
#include "Render\D3D11NV12ToRGBARender.h"
#include "Render\YUV420PToRGBARender.h"

void PlayCore::timeOClock() {
    m_glThread->addTask([this]() {
        if (m_state != PlayState_Playing) {
            return;
        }
        auto frame = m_pDecoder->getOneFrame();
        if (frame != nullptr) {
            m_pRender->render(frame);
            if (m_lastFrame != nullptr) {
                m_pDecoder->freeOneFrame(m_lastFrame);
            }
            m_lastFrame = frame;
        }
        else if(m_pDecoder->isEOF()){
            m_state = PlayState_Stop;
        }
    });
}

void PlayCore::play() {
    m_glThread->addTask([this]() {
        if (m_state != PlayState_Ready && m_state != PlayState_Paused) {
            return;
        }
        m_state = PlayState_Playing;
        m_pClock->start();
    });
}

void PlayCore::pause() {
    m_glThread->addTask([this]() {
        if (m_state != PlayState_Playing) {
            return;
        }
        m_state = PlayState_Paused;
    });
}

void PlayCore::refreshCurrentFrame() {
    m_glThread->addTask([this]() {
        if (m_state == PlayState_Playing) {
            return;
        }
        if (m_lastFrame != nullptr) {
            m_pRender->render(m_lastFrame);
        }
    });
}

PlayState PlayCore::getState() {
    return m_state;
}

PlayCore::PlayCore(const std::string& filePath, long wndId){
    m_pRender = new YUV420PToRGBARender(wndId);
    m_pDecoder = new FFVideoDecoder(filePath);

    m_glThread = new TaskThread;
    m_glThread->addTask([this]() {
        m_pRender->init(); 
        m_state = PlayState_Ready;
        m_pClock = new OClock(33, std::bind(&PlayCore::timeOClock, this));
    });
}

PlayCore::~PlayCore(){
    m_glThread->stop();
    if (m_lastFrame != nullptr) {
        m_pDecoder->freeOneFrame(m_lastFrame);
    }
    delete m_pRender;
    delete m_pDecoder;
}