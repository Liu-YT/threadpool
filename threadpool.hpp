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

template <typename T>
class ThreadPool
{
public:
    // 构造函数
    ThreadPool(int);
    // 析构函数
    ~ThreadPool();
    // 添加任务
    bool addTask(T *task);

private:
    // 工作线程运行函数
    static void *work(void *arg);

    void run();

    // 追踪工作线程
    std::vector<std::thread> workers;
    // 任务队列
    std::queue<T *> tasks;

    std::mutex queueMutex;

    std::condition_variable condition;

    bool stop;
};

template <typename T>
ThreadPool<T>::ThreadPool(int size) : stop(false)
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

template <typename T>
ThreadPool<T>::~ThreadPool()
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

template <typename T>
bool ThreadPool<T>::addTask(T *task)
{
    queueMutex.lock();
    tasks.push(task);
    queueMutex.unlock();
    condition.notify_one();
    return true;
}

template <typename T>
void *ThreadPool<T>::work(void *arg)
{
    ThreadPool *pool = (ThreadPool *)arg;
    pool->run();
    return pool;
}

template <typename T>
void ThreadPool<T>::run()
{
    while (!stop)
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        condition.wait(lock, [this] { return !this->tasks.empty(); });
        if (tasks.empty())
        {
            continue;
        }
        T *task = tasks.front();
        tasks.pop();
        if (task)
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            std::cout << "Thread " << std::this_thread::get_id() << " ";
            task->process();
        }
    }
}
#endif
