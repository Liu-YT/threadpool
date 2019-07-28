#include "threadpool.hpp"
#include <iostream>
using namespace std;
#define TASKSNUM 1024


class Task {

public:
    void process() {
        cout << "do something" << endl;
    }

};


int main() {
    ThreadPool<Task> pool(4);
    
    vector<Task*> tasks;
    for(int i = 0; i < TASKSNUM; i++) {
        Task *task = new Task();
        tasks.push_back(task);
        delete task;
    }
    return 0;
}