#include "threadpool.hpp"
#include <iostream>
using namespace std;
#define TASKSNUM 1000

// class Task
// {

// public:
//     void process()
//     {
//         cout << "do something" << endl;
//     }
// };

void task(int i) {
    cout << i << endl;
}

int main()
{
    ThreadPool pool(4);

    for (int i = 0; i < TASKSNUM; i++)
    {
        pool.addTask(std::bind(task, i));
    }
    return 0;
}