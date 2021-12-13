#pragma once
#include <functional>
#include <vector>
#include <queue>
#include "base.hpp"

using namespace RTSTasks;

namespace RTSSCheduler 
{
    class SlackStealingScheduler : public BaseScheduler
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
            SlackStealingScheduler() {}

            std::vector<unsigned> ap_proc_times_zero_H;	// a.k.a A*(t)
            std::vector<std::vector<unsigned>> ap_proc_time_per_level;	// a.k.a Ai(t)
            unsigned ap_processing_available;	// a.k.a last calculated A*(s, t)
        
		private:
            // Parameters
            unsigned H = 1;        // hyperperiod (a.k.a LCM from periods)
            unsigned abs_time = 0; // absolute time in timeline

            // Containers
            std::vector<Task> periodic_tasks;

            std::priority_queue<Task, std::vector<Task>, OrderTaskByArrivalTime> periodic_arriving;  // order by arrival time
            std::priority_queue<Task, std::vector<Task>, OrderTaskByArrivalTime> aperiodic_arriving; // order by arrival time
            
            std::priority_queue<Task, std::vector<Task>, OrderTaskByPriority> periodic_processing;  // order by priority
            std::priority_queue<Task, std::vector<Task>, OrderTaskByPriority> aperiodic_processing; // order by priority

            // Acumulators
			unsigned              ap_processing_acumulator; // A(s)
			std::vector<unsigned> inactive_acumulators;     // I[i](s)

            // Scheduler Specific functions
            int getSlackTimeAvaiable(unsigned t);
			
            void resetAcumulators();
			
            void updateProcessingQueues();
			
            Task chooseTaskToProcess();
			
            void processTask(Task& task);
			
            void updateAcumulators(const Task& task);
            
            void updateAcumulators(unsigned cur_priority);
			
            void updateAperiodicProcessingAvailable();
    };
}