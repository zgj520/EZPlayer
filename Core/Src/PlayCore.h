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

    void play();

    void pause();

    void refreshCurrentFrame();

    PlayState getState();

private:
    void timeOClock();

private:
    FFVideoDecoder* m_pDecoder = nullptr;
    VideoBaseRender* m_pRender = nullptr;
    PlayState m_state = PlayState_None;

    OClock* m_pClock = nullptr;
    TaskThread* m_glThread = nullptr;
    AVFrame* m_lastFrame = nullptr;
};