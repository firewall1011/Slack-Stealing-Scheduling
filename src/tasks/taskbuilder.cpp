#include "taskbuilder.hpp"

namespace RTSTasks 
{
    // Builders
    Task TaskBuilder::createTask(unsigned arrival_time, unsigned computation_cost)
    {
        if(arrival_time < 0)
            throw std::runtime_error(std::string("Task Builder: invalid value for arrival time."));

        if(computation_cost < 1)
            throw std::runtime_error(std::string("Task Builder: invalid value for task cost."));
        
        return Task(arrival_time, computation_cost, 0, 0, 0, 0, 0);
    }

    Task createHardTask(unsigned arrival_time, unsigned computation_cost, unsigned deadline)
    {   
        if(arrival_time < 0)
            throw std::runtime_error(std::string("Task Builder: invalid value for arrival time."));

        if(computation_cost < 1)
            throw std::runtime_error(std::string("Task Builder: invalid value for task cost."));

        if(deadline < computation_cost)
            throw std::runtime_error(std::string("Task Builder: invalid value for task deadline."));

        return Task(arrival_time, computation_cost, deadline, 0, 0, 0, 0);
    }

    Task TaskBuilder::createPeriodicTask(unsigned arrival_time, unsigned computation_cost, unsigned period, unsigned deadline)
    {   
        if(arrival_time < 0)
            throw std::runtime_error(std::string("Task Builder: invalid value for arrival time."));

        if(computation_cost < 1)
            throw std::runtime_error(std::string("Task Builder: invalid value for task cost."));

        if(period < computation_cost)
            throw std::runtime_error(std::string("Task Builder: invalid value for period"));

        if(deadline < computation_cost || deadline > period)
            throw std::runtime_error(std::string("Task Builder: invalid value for task deadline."));

        return Task(arrival_time, computation_cost, deadline, 0, 0, period, 0);
    }

    Task TaskBuilder::createPeriodicJob(unsigned arrival_time, unsigned computation_cost, unsigned period, unsigned deadline, unsigned j)
    {   
        if(arrival_time < 0)
            throw std::runtime_error(std::string("Task Builder: invalid value for arrival time."));

        if(computation_cost < 1)
            throw std::runtime_error(std::string("Task Builder: invalid value for task cost."));

        if(period < computation_cost)
            throw std::runtime_error(std::string("Task Builder: invalid value for period."));

        if(deadline < computation_cost || deadline > period)
            throw std::runtime_error(std::string("Task Builder: invalid value for task deadline."));

        return Task((arrival_time+period*(j-1)), computation_cost, deadline, 0, 0, period, j);
    }
    
    Task TaskBuilder::createPeriodicJob(const Task& t, unsigned j)
    {
        return Task((t.arrival_time+t.period*(j-1)), t.computation_cost, t.deadline, 0, 0, t.period, j);
    }
}