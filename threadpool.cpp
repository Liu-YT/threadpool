#include "threadpool.hpp"

template <typename T>
ThreadPool<T>::ThreadPool(int size) : stop(false) {
    if(size <= 0) {
        throw std::exception();
    }
    for(int i = 0; i < size; i++) {
        workers.emplace_back(work, this);
    }
}

template <typename T>
ThreadPool<T>::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        stop = true;
    }
    condition.notify_all();
    for(auto &worker : workers) {
        worker.join();
    }
}

template <typename T>
bool ThreadPool<T>::addTask(T* task) {
    queueMutex.lock();
    tasks.push(task);
    queueMutex.unlock();
    condition.notify_one();
    return true;
}

template <typename T>
void *ThreadPool<T>::work(void *arg) {
    ThreadPool *pool = (ThreadPool *)arg;
    pool->run();
    return pool;
}

template <typename T>
void ThreadPool<T>::run() {
    while(!stop) {
        std::unique_lock<std::mutex> lock(queueMutex);
        condition.wait(lock, [this] { return !this->tasks.empty();});
        if(tasks.empty()) {
            continue;
        }
        T* task = tasks.front();
        tasks.pop();
        if(task) {
            std::cout << "Thread " << getpid() << " ";
            task->process();
        }
    }
}