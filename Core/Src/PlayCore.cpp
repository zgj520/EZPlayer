#include "PlayCore.h"
#include "Render\BMPImageRender.h"
#include "Render\D3D11NV12ToRGBARender.h"
#include "Render\YUV420PToRGBARender.h"

void PlayCore::timeOClock() {
    m_glThread->addTask([this]() {
        if (m_state != EZCore::PlayState_Playing) {
            if (m_lastFrame != nullptr) {
                m_pRender->render(m_lastFrame);
            }
            return;
        }
        showNextFrame();
    });
}

void PlayCore::showNextFrame() {
    MediaInfo info;
    m_pDecoder->getMediaInfo(info);
    int64_t timesample = 0;
    auto frame = m_pDecoder->getOneFrame(timesample);
    if (frame != nullptr) {
        m_pRender->render(frame);
        if (m_lastFrame != nullptr) {
            m_pDecoder->freeOneFrame(m_lastFrame);
        }
        m_lastFrame = frame;
    }
    else if (m_pDecoder->isEOF()) {
        m_state = EZCore::PlayState_EOF;
        timesample = info.duration;
    }

    if (m_playcbk) {
        printf("time:%lld", timesample);
        m_playcbk(timesample, info.duration);
    }
}

void PlayCore::play(EZCore::PLAY_CALLBACK cbk) {
    m_glThread->addTask([this, cbk]() {
        if (m_state != EZCore::PlayState_Ready && m_state != EZCore::PlayState_Paused) {
            return;
        }
        m_state = EZCore::PlayState_Playing;
        m_playcbk = cbk;
        m_pClock->start();
    });
}

bool PlayCore::pause() {
    m_glThread->addSyncTask([this]() {
        if (m_state != EZCore::PlayState_Playing) {
            return false;
        }
        if (m_state != EZCore::PlayState_Playing) {
            return false;
        }
        m_state = EZCore::PlayState_Paused;
    });
    return true;
}

bool PlayCore::resume() {
    m_glThread->addSyncTask([this]() {
        if (m_state == EZCore::PlayState_Paused) {
            m_state = EZCore::PlayState_Playing;
            return true;
        }
        else if (m_state == EZCore::PlayState_EOF) {
            auto ret = m_pDecoder->seekTime(0);
            if (ret) {
                m_state = EZCore::PlayState_Playing;
                return true;
            }
            return false;
        }
    });
    return true;
}

bool PlayCore::seekTime(int64_t time) {
    m_glThread->addTask([this, time]() {
        if (m_state == EZCore::PlayState_None) {
            return false;
        }
        auto temp = m_state;
        m_state == EZCore::PlayState_Paused;
        auto ret = m_pDecoder->seekTime(time);
        if (ret) {
            showNextFrame();
        }
        m_state = temp;
    });
    return true;
}

void PlayCore::refreshCurrentFrame() {
    m_glThread->addTask([this]() {
        if (m_state == EZCore::PlayState_Playing) {
            return;
        }
        if (m_lastFrame != nullptr) {
            m_pRender->render(m_lastFrame);
        }
    });
}

EZCore::PlayState PlayCore::getState() {
    return m_state;
}

PlayCore::PlayCore(const std::string& filePath, long wndId){
    m_pRender = new YUV420PToRGBARender(wndId);
    m_pDecoder = new FFVideoDecoder(filePath);

    m_glThread = new TaskThread;
    m_glThread->addTask([this]() {
        m_pRender->init(); 
        m_state = EZCore::PlayState_Ready;
        m_pClock = new OClock(33, std::bind(&PlayCore::timeOClock, this));
    });
}

PlayCore::~PlayCore(){
    m_glThread->stop();
    if (m_lastFrame != nullptr) {
        m_pDecoder->freeOneFrame(m_lastFrame);
    }
    delete m_pClock;
    delete m_pDecoder;
    delete m_glThread;
    delete m_pRender;

}