#include <queue>
#include <numeric>
#include <algorithm>
#include <iostream>
#include <stdexcept>
#include "rateMonotonic.hpp"
#include "../utils/lcm.hpp"

using namespace RTSTasks;


// Helpers Functions
int _getPeriodicInstanceNumber(const Task& task, unsigned t)
{
    return std::max<int>(0,t-task.arrival_time) / task.period;
}

int _getTaskReadyWorkAt(const Task& task, unsigned j)
{
    return j * task.computation_cost;
}

int _getPeriodicInstanceDeadline(const Task& task, unsigned j)
{
    return (task.arrival_time + task.period*(j-1)) + task.deadline;
}

namespace RTSSCheduler 
{
    // Pré-configurantions: Task Initialization
    void RateMonotonicScheduler::preloadTask(Task t)
    {   
        if(!t.isPeriodic())
            throw std::runtime_error(std::string("Rate Monotonic Scheduler: RTS only accepts periodic tasks."));
        
        this->periodic_tasks.push_back(t);
    }
            
    void RateMonotonicScheduler::preloadTask(std::vector<Task> tasks)
    {
        for(auto t: tasks)
            this->preloadTask(t);
    }

    void RateMonotonicScheduler::prepareScheduler()
    {
        // First step: order functions by period time (fixed priority)
        std::sort(this->periodic_tasks.begin(), this->periodic_tasks.end(), sortTaskByLowPeriodTime);

        // Second step: compute the hyperperiod of all periodic functions
        for(auto t: this->periodic_tasks)
            this->H = Utils::lcm(t.period, this->H);

        std::cout << "Hyperperiod is " << H << std::endl;

        // Third step: calculate periodic ready (Pi(t)) work for each task to verify if is feasible
        std::vector<int> high_priority_ready_work(H+1);
        std::vector<int> task_ready_work(H+1); 

        for(auto task: this->periodic_tasks)
        {   
            unsigned j = 0; // a.k.a j-th job of i-th periodic task
            for(auto t = 0; t <= this->H; t++)
            {   
                if(task.arrival_time + task.period*(j) == t)
                    j += 1;

                // unsigned        j = _getPeriodicInstanceNumber(task, t); 
                int task_deadline    = _getPeriodicInstanceDeadline(task, j);
                int task_next_period = task.arrival_time + task.period*(j);

                task_ready_work[t] = _getTaskReadyWorkAt(task, j) + high_priority_ready_work[t];

                std::cout << " t:" << t << " j:" <<  j << " dead:" << task_deadline << " next:" << task_next_period << " pit:" << task_ready_work[t] << std::endl;

                // If is Tij deadline time and task_ready_work[t] > t then the schedule is not feasible
                if(task_deadline == t && task_ready_work[t] > t)
                    throw std::runtime_error(std::string("Rate Monotonic Scheduler: not feasible set of tasks."));
            }
            
            for(auto work: task_ready_work)
                std::cout << work << " "; std::cout << std::endl;
            
            high_priority_ready_work = task_ready_work;
        } 
    }

    // Running Scheduler
    void RateMonotonicScheduler::start()
    {
        // Do nothing
    }

    void RateMonotonicScheduler::tick()
    {
        // Do nothing
    }

    void RateMonotonicScheduler::addOnlineTask(Task t)
    {
        // Do nothing
    }
}