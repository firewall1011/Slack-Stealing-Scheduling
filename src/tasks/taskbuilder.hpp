#pragma once
#include "task.hpp"

namespace RTSTasks
{
    class TaskBuilder
    {
        public:
            // Builders
            static Task createTask(unsigned arrival_time, unsigned computation_cost);

            static Task createHardTask(unsigned arrival_time, unsigned computation_cost, unsigned deadline);

            static Task createPeriodicTask(unsigned arrival_time, unsigned period, unsigned computation_cost, unsigned deadline);

            static Task createPeriodicJob(unsigned arrival_time, unsigned period, unsigned computation_cost, unsigned deadline, unsigned j);
            
            static Task createPeriodicJob(const Task& t, unsigned j);
    };   
}