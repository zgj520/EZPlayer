#pragma once
#include "Render\VideoBaseRender.h"
#include "Decode\FFVideoDecoder.h"
#include "Decode\FFAudioDecoder.h"
#include "../../Utils/Src/Thread/TaskThread.h"
#include "../Interface/CoreInterface.h"
#include "OClock\OClock.h"
#include "Render\Audio\AudioPlayer.h"
#include <string>

class PlayCore
{
public:
    PlayCore(const std::string& filePath, long wndId);

    ~PlayCore();

    void play(EZCore::PLAY_CALLBACK cbk);

    bool pause();

    bool resume();

    bool seekTime(int64_t time);

    void refreshCurrentFrame();

    EZCore::PlayState getState();

private:
    void timeOClock();

    void showNextFrame();

    bool initRender(AVPixelFormat format);

private:
    long m_wndId = 0;
    FFVideoDecoder* m_pVideoDecoder = nullptr;
    FFAudioDecoder* m_pAudioDecoder = nullptr;
    VideoBaseRender* m_pRender = nullptr;
    AudioPlayer* m_AudioRender = nullptr;
    EZCore::PlayState m_state = EZCore::PlayState_None;

    OClock* m_pClock = nullptr;
    TaskThread* m_glThread = nullptr;
    AVFrame* m_lastFrame = nullptr;

    EZCore::PLAY_CALLBACK m_playcbk = nullptr;
};