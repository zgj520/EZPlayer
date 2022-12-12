#include "PlayCore.h"
#include "Render\BMPImageRender.h"
#include "Render\D3D11NV12ToRGBARender.h"
#include "Render\YUV420PToRGBARender.h"
#include "Render\NV12ToRGBARender.h"
#include "Render\D3D11DXRender.h"

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
    EZCore::MediaInfo info;
    m_pVideoDecoder->getMediaInfo(info);
    int64_t timesample = 0;
    auto videoframe = m_pVideoDecoder->getOneFrame(timesample);
    if (videoframe != nullptr) {
        m_pRender->render(videoframe);
        if (m_lastFrame != nullptr) {
            m_pVideoDecoder->freeOneFrame(m_lastFrame);
        }
        m_lastFrame = videoframe;
    }

    int64_t audioTime = 0;
    auto audioframe = m_pAudioDecoder->getOneFrame(audioTime);
    if (audioframe != nullptr) {
        auto type = (AVSampleFormat)audioframe->format;
        //m_AudioRender->WriteFLTP((float*)audioframe->data[0], (float*)audioframe->data[1], audioframe->nb_samples);
    }
    
    if (m_pAudioDecoder->isEOF() && m_pVideoDecoder->isEOF()) {
        m_state = EZCore::PlayState_EOF;
        timesample = info.duration;
    }

    if (m_playcbk) {
        m_playcbk(timesample, info.duration);
    }
}

bool PlayCore::initRender(AVPixelFormat format) {
    if (m_pRender != nullptr) {
        if (m_pRender->getRenderType() == format) {
            return m_pRender;
        }
        else {
            delete m_pRender;
            m_pRender = nullptr;
        }
    }
    switch (format) {
    case AV_PIX_FMT_YUV420P:
        m_pRender = new YUV420PToRGBARender(m_wndId);
        break;
    case AV_PIX_FMT_NV12:
        m_pRender = new NV12ToRGBARender(m_wndId);
        break;
    case AV_PIX_FMT_NV21:
    case AV_PIX_FMT_QSV:
    case AV_PIX_FMT_CUDA:
        break;
    case AV_PIX_FMT_D3D11:
        m_pRender = new D3D11NV12ToRGBARender(m_wndId) /*new D3D11DXRender((HWND)m_wndId)*/;
        break;
    }
    if (m_pRender == nullptr) {
        return false;
    }
    m_glThread = new TaskThread;
    m_glThread->addTask([this]() {
        m_pRender->init();
        m_state = EZCore::PlayState_Ready;
        m_pClock = new OClock(33, std::bind(&PlayCore::timeOClock, this));
    });
    return true;
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
            auto ret = m_pVideoDecoder->seekTime(0);
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
        m_pVideoDecoder->seekTime(time);
        m_pAudioDecoder->seekTime(time);
        showNextFrame();
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
    m_wndId = wndId;
    m_pVideoDecoder = new FFVideoDecoder(filePath);
    m_pAudioDecoder = new FFAudioDecoder(filePath);
    initRender(AV_PIX_FMT_D3D11/*AV_PIX_FMT_NV12*/);
    // 初始化 AudioPlayer，无论如何固定使用双声道
    m_AudioRender = new AudioPlayer(2, m_pAudioDecoder->getSampleRate());
    m_AudioRender->Start();
}

PlayCore::~PlayCore(){
    m_glThread->stop();
    if (m_lastFrame != nullptr) {
        m_pVideoDecoder->freeOneFrame(m_lastFrame);
    }
    delete m_pClock;
    delete m_pVideoDecoder;
    delete m_pAudioDecoder;
    delete m_glThread;
    delete m_pRender;

}