#ifndef TASK_Q_H
#define TASK_Q_H

#include <condition_variable>
#include <mutex>
#include <queue>
#include <ctime>
#include "logger.h"



namespace biti{

    enum class JobType{
        FILE_SAVE // background job to save file to disk
    };

    struct Task{
        std::time_t arrived; // time we created the job
        JobType type;
        void *arg_1;
        void *arg_2;
    };

    class TaskQueue{
        private:
            std::queue<Task> queue;
            std::mutex queue_lock;
            std::condition_variable notify;
        public:
    
            /*
                Injects an item into the queue
            */
            void push(Task data){
                std::unique_lock<std::mutex> queue_guard(queue_lock);
                queue_guard.lock();
                queue.push(data);
                // unlock the mutex before notifying to avoid waking up a thread and 
                // going to block again
                queue_guard.unlock();
                notify.notify_one();
            }
            
            /*  
                This function blocks until there's data on the queue
                Removes and returns the first item from the queue
            */
            Task get_task(){
                std::unique_lock<std::mutex> queue_guard{queue_lock};
                while(!queue.empty()){
                    notify.wait(queue_guard);
                    auto data = queue.front();
                    queue.pop();
                    queue_guard.unlock();
                    return data;
                }        
            }
    };

}

#endif 