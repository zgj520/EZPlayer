#pragma once
#include "../../Utils/Src/Thread/TaskThread.h"
#include <string>

using OCLOCK_CALLBACK = std::function<void()>;
class OClock
{
public:
    OClock(int64_t interalMSTime, OCLOCK_CALLBACK callBack);
    ~OClock();
    void start();
    void stop();

private:
    TaskThread* m_pClockThread = nullptr;
    OCLOCK_CALLBACK m_callback = nullptr;
    int64_t m_interalMSTime = 0;
    std::atomic_bool m_bStop = { true };

    std::condition_variable m_clockConditionVar;
    std::mutex m_mutex;
};