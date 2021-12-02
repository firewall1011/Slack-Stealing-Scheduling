#include "taskbuilder.hpp"

namespace RTSTasks 
{
    // Builders
    Task TaskBuilder::createTask(unsigned arrival_time, unsigned computation_cost)
    {
        return Task(arrival_time, computation_cost, 0, 0, 0, 0, 0);
    }

    Task createHardTask(unsigned arrival_time, unsigned computation_cost, unsigned deadline)
    {
        return Task(arrival_time, computation_cost, deadline, 0, 0, 0, 0);
    }

    Task TaskBuilder::createPeriodicTask(unsigned arrival_time, unsigned period, unsigned computation_cost, unsigned deadline)
    {
        return Task(arrival_time, computation_cost, deadline, 0, 0, period, 0);
    }

    Task TaskBuilder::createPeriodicJob(unsigned arrival_time, unsigned period, unsigned computation_cost, unsigned deadline, unsigned j)
    {
        return Task((arrival_time+period*(j-1)), computation_cost, deadline, 0, 0, period, j);
    }
    
    Task TaskBuilder::createPeriodicJob(const Task& t, unsigned j)
    {
        return Task((t.arrival_time+t.period*(j-1)), t.computation_cost, t.deadline, 0, 0, t.period, j);
    }
}