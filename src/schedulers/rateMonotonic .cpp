#include <queue>
#include <numeric>
#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <limits>
#include "rateMonotonic.hpp"
#include "../utils/lcm.hpp"

using namespace RTSTasks;


// Helpers Functions
unsigned _getPeriodicInstanceNumber(const Task& task, unsigned t)
{
    return std::max<unsigned>(0,int(t)-task.arrival_time) / task.period;
}

unsigned _getTaskReadyWorkAt(const Task& task, unsigned j)
{
    return j * task.computation_cost;
}

unsigned _getPeriodicInstanceDeadline(const Task& task, unsigned j)
{
    return (task.arrival_time + task.period*(j-1)) + task.deadline;
}

unsigned _getAperiodicProcessingTime(const Task& task, unsigned s, unsigned t)
{
    return 0; // TODO: it's just mocked
}

void _emptyQueue(std::priority_queue<Task>& queue)
{
    while(!queue.empty()) queue.pop();
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

        // Debug the Hyperperiod
        // std::cout << "Hyperperiod is " << H << std::endl;

        // Third step: calculate periodic ready (Pi(t)) work for each task to verify if is feasible
        std::vector<unsigned> high_priority_ready_work(H+2);
        std::vector<unsigned> task_ready_work(H+2); 
        
        // Fourth step: do at the same time the computantions of A*(t) to avoid useless storage :)
        this->slack_stealer_function.resize(H+1);
        std::fill(this->slack_stealer_function.begin(), this->slack_stealer_function.end(), std::numeric_limits<unsigned>::max());

        for(auto task: this->periodic_tasks)
        {      
            // Compute Pi(t) or task_ready_work
            unsigned j = 0; // a.k.a j-th job of i-th periodic task
            for(auto t = 0; t <= this->H+1; t++)
            {   
                if(task.arrival_time + task.period*(j) == t)
                    j += 1;

                unsigned task_deadline    = _getPeriodicInstanceDeadline(task, j);
                unsigned task_next_period = task.arrival_time + task.period*(j);

                task_ready_work[t] = _getTaskReadyWorkAt(task, j) + high_priority_ready_work[t];
                
                // Use to debug the tasks :)
                // std::cout << " t:" << t << " j:" <<  j << " dead:" << task_deadline << " next:" << task_next_period << " pit:" << task_ready_work[t] << std::endl;

                // If is Tij deadline time and task_ready_work[t] > t then the schedule is not feasible
                if(task_deadline == t && task_ready_work[t] > t)
                    throw std::runtime_error(std::string("Rate Monotonic Scheduler: not feasible set of tasks."));
            }

            // for(auto work: task_ready_work)
            //    std::cout << work << " "; std::cout << std::endl;

            // Compute Ai(t) and A*(t)
            j = 1;
            for(auto t = 0; t <= this->H; t++)
            {    
                unsigned task_deadline = std::min<unsigned>(this->H, _getPeriodicInstanceDeadline(task, j));
                if(task_deadline == t)
                    j += 1;
                task_deadline = std::min<unsigned>(this->H, _getPeriodicInstanceDeadline(task, j));

                // Debug Ai(t) values
                // std::cout << "t: " << t << "\tj:"<< j << "\tdead:" << task_deadline << "\tw:" << task_ready_work[task_deadline-1] << "\tAi:" << task_deadline - task_ready_work[task_deadline-1] << std::endl;

                this->slack_stealer_function[t] = std::min(this->slack_stealer_function[t], (task_deadline-task_ready_work[task_deadline-1]));
            }

            // Print A*t()
            // for(auto slack: this->slack_stealer_function)
            //    std::cout << slack << " "; std::cout << std::endl;

            high_priority_ready_work = task_ready_work;
        } 
    }

    // Running Scheduler
    void RateMonotonicScheduler::start()
    {
        _emptyQueue(periodic_arriving);
        for (const Task& periodic : periodic_tasks)
        {
            periodic_arriving.push(periodic);
        }
        abs_time = 0;
    }

    void RateMonotonicScheduler::tick()
    {
        // First step: add all ready tasks to processing queue
        Task task = periodic_arriving.top();
        while (task.arrival_time == abs_time)
        {
            periodic_arriving.pop();
            periodic_processing.push(task);
            
            task = periodic_arriving.top();
        }

        task = aperiodic_arriving.top();
        while (task.arrival_time == abs_time)
        {
            aperiodic_arriving.pop();
            aperiodic_processing.push(task);
            
            task = aperiodic_arriving.top();
        }
        
        // Second step: choose task to process
        Task task_to_process = aperiodic_processing.top();
        
        unsigned earliest_deadline = 0;
        bool enough_ap_processing = _getAperiodicProcessingTime(task_to_process, abs_time, earliest_deadline) > 0;
        
        if (aperiodic_processing.empty() || !enough_ap_processing)
        {
            task_to_process = periodic_processing.top();
        }

        // Third step: process task
        bool doneProcessing = task_to_process.compute();
        
        // Forth step: remove task from queue if done
        if(doneProcessing)
        {
            if(task.isPeriodic())
            {
                periodic_processing.pop();
                task_to_process.computed = 0;
                task_to_process.arrival_time += task_to_process.period;
                periodic_arriving.push(task_to_process);
            }
            else
            {
                aperiodic_processing.pop();
            }
        }
        
        // Fifth step: advance time
        abs_time += 1;
    }

    void RateMonotonicScheduler::addOnlineTask(Task t)
    {
        // Do nothing
    }
}