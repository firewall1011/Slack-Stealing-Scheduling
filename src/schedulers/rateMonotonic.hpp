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
            unsigned abs_time = 0; // absolute time in timeline

            // Containers
            std::vector<Task> periodic_tasks;

            std::priority_queue<Task> periodic_arriving; // order by arrival time
            std::priority_queue<Task> aperiodic_arriving;   // order by arrival time
            
            std::priority_queue<Task> periodic_processing;  // order by priority
            std::priority_queue<Task> aperiodic_processing; // order by priority

		public:
            std::vector<unsigned> ap_proc_time_zero_H;      // a.k.a A*(t)
            std::vector<std::vector<unsigned>> ap_proc_time_per_level;   // a.k.a Ai(t)
            unsigned ap_processing_available;               // a.k.a last calculated A*(s, t)
    };
}