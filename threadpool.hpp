#ifndef THREAD_POOL_HPP
#define THREAD_POOL_HPP

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <iostream>
#include <unistd.h>
#include <stdexcept>
#include <chrono>
#include <functional>

typedef std::function<void(void)> Task;

class ThreadPool
{
public:
    // 构造函数
    ThreadPool(int);
    // 析构函数
    ~ThreadPool();
    // 添加任务
    bool addTask(Task task);

private:
    // 工作线程运行函数
    static void *work(void *arg);

    void run();

    // 追踪工作线程
    std::vector<std::thread> workers;
    // 任务队列
    std::queue<Task> tasks;

    std::mutex queueMutex;

    std::condition_variable condition;

    bool stop;
};

ThreadPool::ThreadPool(int size) : stop(false)
{
    if (size <= 0)
    {
        throw std::exception();
    }
    for (int i = 0; i < size; i++)
    {
        workers.emplace_back(work, this);
    }
}

ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        stop = true;
    }
    condition.notify_all();
    for (auto &worker : workers)
    {
        worker.join();
    }
}

bool ThreadPool::addTask(Task task)
{
    queueMutex.lock();
    tasks.push(task);
    queueMutex.unlock();
    condition.notify_one();
    return true;
}

void *ThreadPool::work(void *arg)
{
    ThreadPool *pool = (ThreadPool *)arg;
    pool->run();
    return pool;
}

void ThreadPool::run()
{
    while (true)
    {
        Task task;
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            condition.wait(lock, [this] { return !this->tasks.empty() || this->stop; });
            if (this->stop && this->tasks.empty())
            {
                return;
            }
            task = std::move(this->tasks.front());
            this->tasks.pop();
        }
        // task();

        // just for test
        std::cout << "Thread " << std::this_thread::get_id() << " sleep" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "Thread " << std::this_thread::get_id() << " ";
        task();
    }
}
#endif
