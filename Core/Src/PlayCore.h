#pragma once
#include "Render\VideoBaseRender.h"
#include "Decode\FFVideoDecoder.h"
#include "../../Utils/Src/Thread/TaskThread.h"
#include "../Interface/CoreInterface.h"
#include "OClock\OClock.h"
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

private:
    FFVideoDecoder* m_pDecoder = nullptr;
    VideoBaseRender* m_pRender = nullptr;
    EZCore::PlayState m_state = EZCore::PlayState_None;

    OClock* m_pClock = nullptr;
    TaskThread* m_glThread = nullptr;
    AVFrame* m_lastFrame = nullptr;

    EZCore::PLAY_CALLBACK m_playcbk = nullptr;
};