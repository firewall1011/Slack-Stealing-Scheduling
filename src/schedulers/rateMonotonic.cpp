#include <queue>
#include <numeric>
#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <limits>
#include "rateMonotonic.hpp"
#include "utils/lcm.hpp"

using namespace RTSTasks;


// Helpers Functions
inline unsigned _getPeriodicInstanceNumber(const Task& task, unsigned t)
{
    return std::max<unsigned>(0,int(t)-task.arrival_time) / task.period;
}

inline unsigned _getTaskReadyWorkAt(const Task& task, unsigned j)
{
    return j * task.computation_cost;
}

inline unsigned _getPeriodicInstanceDeadline(const Task& task, unsigned j)
{
    return (task.arrival_time + task.period*(j-1)) + task.deadline;
}

namespace RTSSCheduler 
{
    // Pré-configurantions: Task Initialization
    void RateMonotonicScheduler::preloadTask(Task t)
    {   
        if(!t.isPeriodic())
            throw std::runtime_error(std::string("Rate Monotonic Scheduler: RMS only accepts periodic tasks."));
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
        std::sort(this->periodic_tasks.begin(), this->periodic_tasks.end(), sortTaskByRateMonotonic);
        
        for(int i = 0; i < this->periodic_tasks.size(); i++)
            periodic_tasks[i].priority = i;

        // Second step: compute the hyperperiod of all periodic functions
        for(auto t: this->periodic_tasks)
            this->H = Utils::lcm(t.period, this->H);

        // Debug the Hyperperiod
        if(this->FLAG_DEBUG)
        {
            std::cout << "[i] prepareScheduler() debug info:" << std::endl;
            std::cout << "[i] | -- Hyperperiod H is " << H << " ticks" << std::endl;
        }   

        // Third step: calculate periodic ready (Pi(t)) work for each task to verify if is feasible
        std::vector<unsigned> high_priority_ready_work(H+2);
        std::vector<unsigned> task_ready_work(H+2); 

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
                
                // If is Tij deadline time and task_ready_work[t] > t then the schedule is not feasible
                if(task_deadline == t && task_ready_work[t] > t)
                    throw std::runtime_error(std::string("Rate Monotonic Scheduler: not feasible set of periodic tasks."));
            }

            high_priority_ready_work = task_ready_work;

            if(this->FLAG_DEBUG){
                std::cout << "[i] | -- A[" << task.priority << "](t): "; 
                for(auto value : high_priority_ready_work) std::cout << value << " ";
                std::cout << std::endl;
            }
        } 
    }
    
    // Running Scheduler
    void RateMonotonicScheduler::start()
    {    
		while(!periodic_arriving.empty()) periodic_arriving.pop();

        for (const Task& periodic : periodic_tasks)
            periodic_arriving.push(periodic);
        
        abs_time = 0;
    }

    void RateMonotonicScheduler::tick()
    {
        // First step: add all ready tasks to processing queue
        this->_updateProcessingQueues();

        std::cout << "[ ]" << std::endl;
        std::cout << "[ ]" << std::endl;
        std::cout << "[t] Time:" << this->abs_time << " | Relative time:" << this->abs_time % this->H << std::endl;

        if(this->FLAG_DEBUG)
        {
            std::cout << "[i] tick() debug info:" << std::endl;
            std::cout << "[i] | -- Periodic Arriving Size: " << this->periodic_arriving.size() << std::endl;
            std::cout << "[i] | -- Periodic Processing Size: " << this->periodic_processing.size() << std::endl;
        }  

        if (!this->periodic_processing.empty())
        {
            // Second step: choose task to process
            Task task_to_process = this->_chooseTaskToProcess();

            // Third step: process task
            this->_processTask(task_to_process);   
            
            std::cout << "[t] Running " << task_to_process.name << ((task_to_process.isPeriodic()) ? " [Periodic]" : " [Aperiodic]") << std::endl;
            
            if(task_to_process.isPeriodic())
                std::cout << "[t] | -- Priority=" << task_to_process.priority 
                            << " Computed=" << task_to_process.computed << "/" << task_to_process.computation_cost
                            << " Arrived="  << task_to_process.arrival_time
                            << " Deadline=" << task_to_process.arrival_time + task_to_process.deadline;  
            else 
                std::cout << "[t] | -- Computed=" << task_to_process.computed << "/" << task_to_process.computation_cost
                            << " Arrived="  << task_to_process.arrival_time;

            if(task_to_process.computed == task_to_process.computation_cost)
                std::cout << std::endl << "[t] | -- Task Finished :)";
        }
        else
        {
            std::cout << "[t] Processor Idle... ";
        }
        
        // Fourth step: advance time
        this->abs_time++;
    }

    void RateMonotonicScheduler::addOnlineTask(Task t)
    {
        // Do nothing
    }

    // Scheduler Internal Functions
    void RateMonotonicScheduler::_updateProcessingQueues()
	{   
		while(!this->periodic_arriving.empty() && this->periodic_arriving.top().arrival_time == this->abs_time)
        {
            Task task = this->periodic_arriving.top();
			this->periodic_arriving.pop();
			this->periodic_processing.push(task);
        }
	}

	Task RateMonotonicScheduler::_chooseTaskToProcess()
	{
        Task task = this->periodic_processing.top();
		this->periodic_processing.pop();
        return task;
	}

	void RateMonotonicScheduler::_processTask(Task& task)
	{
		bool doneProcessing = task.compute();

        
		if(!doneProcessing)
        {
		    this->periodic_processing.push(task);
        }
		else
		{
            Task new_task = task;
            new_task.arrival_time += new_task.period;
            new_task.computed = 0;
            new_task.finish_time = 0;
            new_task.response_time = 0;
            new_task.job_instance += 1;
            this->periodic_arriving.push(new_task);
		}
	}
}