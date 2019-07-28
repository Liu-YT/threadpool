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

template <typename T>
class ThreadPool {
public:
    // 构造函数
    ThreadPool(int);
    // 析构函数
    ~ThreadPool();
    // 添加任务
    bool addTask(T* task);
    
private:
    // 工作线程运行函数
    void *work(void *arg);

    void run();

    // 追踪工作线程    
    std::vector<std::thread> workers;
    // 任务队列
    std::queue<T*> tasks;

    std::mutex queueMutex;

    std::condition_variable condition;

    bool stop;
};


#endif