#include "threadpool.hpp"
#include <iostream>
#include <unistd.h>
using namespace std;
#define TASKSNUM 100


class Task {

public:
    void process() {
        cout << "sleep" << endl;
        sleep(10);
        cout << "do something" << endl;
    }

};


int main() {
    ThreadPool<Task> pool(4);
    
    vector<Task*> tasks;
    for(int i = 0; i < TASKSNUM; i++) {
        Task *task = new Task();
        pool.addTask(task);
        delete task;
    }
    return 0;
}