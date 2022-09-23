#include "TaskThread.h"
static void* threadFunc(void* arg) {
    TaskThread* pThreadHandle = static_cast<TaskThread*>(arg);
    pThreadHandle->run();
    return nullptr;
}

TaskThread::TaskThread() {
    m_pThread = new std::thread(&threadFunc, this);
}

TaskThread::~TaskThread() {
    stop();
}

void TaskThread::addTask(THREADTASK task) {
    if (m_bStop) {
        return;
    }
    std::unique_lock<std::mutex> uniqueLock(m_Mutex);
    m_vtTask.push_back(task);
    m_ConditionVar.notify_one();
}

void TaskThread::run() {
    while (!m_bStop) {
        THREADTASK task = nullptr;
        {
            std::unique_lock<std::mutex> uniqueLock(m_Mutex);
            if (m_vtTask.empty()) {
                m_ConditionVar.wait(uniqueLock);
            }
            else {
                task = m_vtTask.front();
                m_vtTask.erase(m_vtTask.begin());
            }
        }
        if (task == nullptr) {
            continue;
        }
        task();
    }
}

void TaskThread::stop() {
    m_bStop = true;
    m_ConditionVar.notify_all();
    if (m_pThread) {
        if (m_pThread->joinable()) {
            m_pThread->join();
        }
        delete m_pThread;
        m_pThread = nullptr;
    }
    m_vtTask.clear();
}