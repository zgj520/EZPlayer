#include "OClock.h"

OClock::OClock(int64_t interalMSTime, OCLOCK_CALLBACK callBack) {
    m_interalMSTime = interalMSTime;
    m_callback = callBack;
}

void OClock::start() {
    if (!m_bStop) {
        return;
    }
    if (m_pClockThread == nullptr) {
        m_pClockThread = new TaskThread;
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
    delete m_pClockThread;
    m_pClockThread = nullptr;
}

OClock::~OClock() {
    stop();
}