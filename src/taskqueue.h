#ifndef TASK_Q_H
#define TASK_Q_H

#include <condition_variable>
#include <mutex>
#include <queue>
#include <ctime>
#include "logger.h"



namespace biti{

    enum class TaskType{
        FILE_SAVE, // background job to save file to disk
        UNKNOWN
    };

    struct Task{
        std::time_t created; // time we created the job
        TaskType type;
        std::string arg;

        Task(): created{0}, type{TaskType::UNKNOWN}, arg{""}
        {}
    };

    

    class TaskQueue{
        private:
            std::queue<Task> queue;
            std::mutex queue_lock;
            std::condition_variable cond_var;
        public:
    
            /*
                Injects an item into the queue
            */
            void push(Task data){
                std::unique_lock<std::mutex> queue_guard(queue_lock);
                queue.push(data);
                // unlock the mutex before notifying to avoid waking up a thread and 
                // going to block again
                queue_guard.unlock();
                cond_var.notify_one();
            }
            
            /*  
                This function blocks until there's data on the queue
                Removes and returns the first item from the queue
            */
            Task get_task(){
                std::unique_lock<std::mutex> queue_guard{queue_lock};
                // due to spurios wake ups we put the con variable waiting in a loop with the empty check
                while(queue.empty()){                    
                    cond_var.wait(queue_guard);                    
                }        
                auto data = queue.front();
                queue.pop();
                queue_guard.unlock();
                return data;
            }

            int size(){
                std::unique_lock<std::mutex> queue_guard{queue_lock};
                return queue.size();
            }
    };

}

#endif 