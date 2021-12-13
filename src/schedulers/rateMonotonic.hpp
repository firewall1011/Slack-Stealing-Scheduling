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
            // Pré-configurantions Implementations
            void preloadTask(Task t);
            void preloadTask(std::vector<Task> t);
            void prepareScheduler();

            // Running Scheduler Impĺementations
            void start();
            void tick();
            void addOnlineTask(Task t);

            // Constructor
            RateMonotonicScheduler() {}
            
		private:
            // Parameters
            unsigned H = 1;        // hyperperiod (a.k.a LCM from periods)
            unsigned abs_time = 0; // absolute time in timeline

            // Containers
            std::vector<Task> periodic_tasks; // Pre-loaded periodic tasks
            std::priority_queue<Task, std::vector<Task>, OrderTaskByArrivalTime> periodic_arriving; // order by arrival time
            std::priority_queue<Task, std::vector<Task>, OrderTaskByPriority> periodic_processing;  // order by priority

            // Scheduler Internal Functions
            void _updateProcessingQueues();
			
            Task _chooseTaskToProcess();
			
            void _processTask(Task& task);
    };
}