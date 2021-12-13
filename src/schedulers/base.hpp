#pragma once
#include <vector>
#include "../tasks/task.hpp"
#include "../tasks/sortingStrategies.hpp"

using namespace RTSTasks;

namespace RTSSCheduler 
{   
    class BaseScheduler
    {
        public:
            // Utilities Flags
            bool FLAG_DEBUG = false;
            
            // Usefull variables
            unsigned T = 0;
            unsigned TIME_FAULTS = 0;
            
            // Pré-configurantions: Task Initialization
            virtual void preloadTask(Task t) = 0;
            
            virtual void preloadTask(std::vector<Task> t) = 0;

            virtual void prepareScheduler() = 0;

            // Running Scheduler
            virtual void start() = 0;

            virtual void tick() = 0;

            virtual void addOnlineTask(Task t) = 0;
    };

    struct OrderTaskByPriority
	{
		bool operator() (Task a, Task b)
		{
			return a.priority > b.priority;
		}
	};

	struct OrderTaskByArrivalTime
	{
		bool operator() (Task a, Task b)
		{
			return a.arrival_time > b.arrival_time;
		}
	};
}