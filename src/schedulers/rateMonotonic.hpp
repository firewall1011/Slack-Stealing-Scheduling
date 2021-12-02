#pragma once
#include <functional>
#include <vector>
#include <queue>
#include "base.hpp"

using namespace RTSTasks;

namespace RTSSCheduler 
{
    class RateMonotonicScheduler : public BaseScheduler
    {   
        public:
            // Pré-configurantions: Task Initialization
            void preloadTask(Task t);
            void preloadTask(std::vector<Task> t);
            void prepareScheduler();

            // Running Scheduler
            void start();
            void tick();
            void addOnlineTask(Task t);

            // Constructor
            RateMonotonicScheduler() {}

        private:
            // Parameters
            unsigned H = 1; // hyperperiodo (a.k.a LCM from periods)

            // COntainers
            std::vector<Task> periodic_tasks;
            std::priority_queue<Task> task_queue;
            // std::priority_queue<Task, std::vector<Task>, decltype(sortTaskByLowPeriodTime)> task_queue(sortTaskByLowPeriodTime);
            std::priority_queue<Task> future_tasks;
            // std::priority_queue<Task, std::vector<Task>, decltype(sortTaskByEarliestArrivalTime)> future_tasks(sortTaskByEarliestArrivalTime);            
    };
}