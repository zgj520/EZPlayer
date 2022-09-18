#include "OClock.h"

OClock::OClock(int64_t interalMSTime, OCLOCK_CALLBACK callBack) {
    m_interalMSTime = interalMSTime;
    m_callback = callBack;

    m_pClockThread = new TaskThread;
}

void OClock::start() {
    if (!m_bStop) {
        return;
    }
    m_pClockThread->addTask([this]() {
        m_bStop = false;
        while (!m_bStop) {
            std::unique_lock<std::mutex> uniqueLock(m_mutex);
            m_clockConditionVar.wait_for(uniqueLock, std::chrono::milliseconds(m_interalMSTime));
            m_callback();
        }
        m_bStop = true;
    });
}
void OClock::stop() {
    m_bStop = true;
    m_pClockThread->stop();
}

OClock::~OClock() {
    m_bStop = true;
    m_pClockThread->stop();
    delete m_pClockThread;
}