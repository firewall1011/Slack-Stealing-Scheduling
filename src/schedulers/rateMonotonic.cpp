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
        
        for(int i = 0; i < this->periodic_tasks.size(); i++)
            periodic_tasks[i].priority = i;

        // Second step: compute the hyperperiod of all periodic functions
        for(auto t: this->periodic_tasks)
            this->H = Utils::lcm(t.period, this->H);

        // Debug the Hyperperiod
        // std::cout << "Hyperperiod is " << H << std::endl;

        // Third step: calculate periodic ready (Pi(t)) work for each task to verify if is feasible
        std::vector<unsigned> high_priority_ready_work(H+2);
        std::vector<unsigned> task_ready_work(H+2); 
        
        // Fourth step: do at the same time the computations of A*(t) to avoid useless storage :)
        this->ap_proc_times_zero_H.resize(H+1);
        this->ap_proc_time_per_level.resize(periodic_tasks.size());
        
        std::fill(this->ap_proc_times_zero_H.begin(), this->ap_proc_times_zero_H.end(), std::numeric_limits<unsigned>::max());

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
			this->ap_proc_time_per_level[task.priority].resize(this->H + 1);
            for(auto t = 0; t <= this->H; t++)
            {    
                unsigned task_deadline = std::min<unsigned>(this->H, _getPeriodicInstanceDeadline(task, j));
                if(task_deadline == t)
                    j += 1;
                task_deadline = std::min<unsigned>(this->H, _getPeriodicInstanceDeadline(task, j));

                // Debug Ai(t) values
                // std::cout << "t: " << t << "\tj:"<< j << "\tdead:" << task_deadline << "\tw:" << task_ready_work[task_deadline-1] << "\tAi:" << task_deadline - task_ready_work[task_deadline-1] << std::endl;
                this->ap_proc_time_per_level[task.priority][t] = task_deadline - task_ready_work[task_deadline-1];
                // This is A*(t)
                this->ap_proc_times_zero_H[t] = std::min(this->ap_proc_times_zero_H[t], this->ap_proc_time_per_level[task.priority][t]);
            }

            // Print A*t()
            // for(auto slack: this->ap_proc_times_zero_H)
            //    std::cout << slack << " "; std::cout << std::endl;

            high_priority_ready_work = task_ready_work;
        } 
    }

    int RateMonotonicScheduler::getSlackTimeAvaiable(unsigned t)
    {
        unsigned min = std::numeric_limits<unsigned>::max();
     
        for (int i = 0; i < this->periodic_tasks.size(); i++)
        {
            min = std::min(this->ap_proc_time_per_level[i][t] - inactive_acumulators[i] - ap_processing_acumulator, min);
        }        

        return min;
    }
    

    // Running Scheduler
    void RateMonotonicScheduler::start()
    {
		while(!periodic_arriving.empty()) periodic_arriving.pop();

        for (const Task& periodic : periodic_tasks)
        {
            periodic_arriving.push(periodic);
        }
        abs_time = 0;
		
		inactive_acumulators.resize(periodic_tasks.size());
    }

	void RateMonotonicScheduler::resetAcumulators()
	{
		std::fill(this->inactive_acumulators.begin(), this->inactive_acumulators.end(), 0);
		ap_processing_acumulator = 0;
	}

	void RateMonotonicScheduler::updateProcessingQueues()
	{
		Task task = this->periodic_arriving.top();
        while (task.arrival_time == this->abs_time)
        {
            this->periodic_arriving.pop();
            this->periodic_processing.push(task);
            
            task = periodic_arriving.top();
        }

        task = this->aperiodic_arriving.top();
        while (task.arrival_time == this->abs_time)
        {
            this->aperiodic_arriving.pop();
            this->aperiodic_processing.push(task);
            
            task = this->aperiodic_arriving.top();
        }
	}

	Task RateMonotonicScheduler::chooseTaskToProcess()
	{

        if(aperiodic_processing.empty())
        {
            return periodic_processing.top();
        }

		Task task = aperiodic_processing.top();

        bool enough_ap_processing = getSlackTimeAvaiable(this->abs_time+1) > 0;
        
        if (!enough_ap_processing)
        {
            task = periodic_processing.top();
        }

		return task;
	}
	
	void RateMonotonicScheduler::updateAcumulators(const Task& task)
	{
		if(!task.isPeriodic())
		{
			ap_processing_acumulator += 1;
		}
		else
		{
			for(int i = task.priority-1; i >= 0; i--)
			{
				inactive_acumulators[i] += 1;
			}
		}
	}

    void RateMonotonicScheduler::updateAcumulators(unsigned cur_priority)
	{
        for(int i = cur_priority-1; i >= 0; i--)
        {
            inactive_acumulators[i] += 1;
        }
	}
	
	void RateMonotonicScheduler::processTask(Task& task)
	{
		bool doneProcessing = task.compute();

		this->updateAcumulators(task);
		

        // remove task from queue if done
        if(doneProcessing)
        {
            if(task.isPeriodic())
            {
                this->periodic_processing.pop();
                task.computed = 0;
                task.arrival_time += task.period;
                this->periodic_arriving.push(task);
            }
            else
            {
                this->aperiodic_processing.pop();
            }
        }
	}

    void RateMonotonicScheduler::tick()
    {

		// if at the beginning of a hyperperiod, reset acumulators
		if(this->abs_time == 0)
			this->resetAcumulators();

        // First step: add all ready tasks to processing queue
        this->updateProcessingQueues();
        
        if (!this->aperiodic_processing.empty() || !this->periodic_processing.empty())
        {
            // Second step: choose task to process
            Task task_to_process = this->chooseTaskToProcess();

            // Third step: process task
            this->processTask(task_to_process);   

            std::cout << this->abs_time << " " << task_to_process.priority << std::endl;

        }
        else
        {
            unsigned cur_priority = this->periodic_tasks.size();
            this->updateAcumulators(cur_priority);
            std::cout << this->abs_time << " " << "idle" << std::endl;
        }
        
        // Forth step: advance time
        this->abs_time++;
		this->abs_time %= this->H;
        
    }

    void RateMonotonicScheduler::addOnlineTask(Task t)
    {
        // Do nothing
    }
}