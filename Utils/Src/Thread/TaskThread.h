#ifndef TASKTHREAD_H_
#define TASKTHREAD_H_
#include <thread>
#include <functional>
#include <vector>
#include <mutex>

using THREADTASK = std::function<void()>;

class TaskThread {

public:
    TaskThread();
    ~TaskThread();
    void addTask(THREADTASK task);

    void run();

    void stop();

private:
    std::thread* m_pThread = nullptr;
    std::atomic_bool m_bStop = { false };
    std::condition_variable m_ConditionVar;
    std::mutex m_Mutex;

    std::vector<THREADTASK> m_vtTask;
};

#endif