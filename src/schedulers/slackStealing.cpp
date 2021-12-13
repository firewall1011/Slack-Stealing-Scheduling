#include <queue>
#include <numeric>
#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <limits>
#include "slackStealing.hpp"
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
    void SlackStealingScheduler::preloadTask(Task t)
    {   
        if(t.isPeriodic())
            this->periodic_tasks.push_back(t);
        else
        {
            t.priority = 0;
            this->aperiodic_arriving.push(t);
        }
    }
            
    void SlackStealingScheduler::preloadTask(std::vector<Task> tasks)
    {
        for(auto t: tasks)
            this->preloadTask(t);
    }

    void SlackStealingScheduler::prepareScheduler()
    {
        // First step: order functions by period time (fixed priority)
        std::sort(this->periodic_tasks.begin(), this->periodic_tasks.end(), sortTaskByLowPeriodTime);
        
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
                
                // If is Tij deadline time and task_ready_work[t] > t then the schedule is not feasible
                if(task_deadline == t && task_ready_work[t] > t)
                    throw std::runtime_error(std::string("Slack Stealing Scheduler: not feasible set of tasks."));
            }

            if(this->FLAG_DEBUG){
                std::cout << "[i] | -- A[" << task.priority << "](t): "; 
                for(auto value : task_ready_work) std::cout << value << " ";
                std::cout << std::endl;
            }

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
                this->ap_proc_time_per_level[task.priority][t] = task_deadline - task_ready_work[task_deadline-1];
                
                // This is A*(t)
                this->ap_proc_times_zero_H[t] = std::min(this->ap_proc_times_zero_H[t], this->ap_proc_time_per_level[task.priority][t]);
            }

            if(this->FLAG_DEBUG){
                std::cout << "[i] | -- A*[" << task.priority << "](t): "; 
                for(auto value : ap_proc_time_per_level[task.priority]) std::cout << value << " ";
                std::cout << std::endl;
            }

            high_priority_ready_work = task_ready_work;
        } 
    }

    int SlackStealingScheduler::getSlackTimeAvaiable(unsigned t)
    {
        unsigned min = std::numeric_limits<unsigned>::max();
     
        for (int i = 0; i < this->periodic_tasks.size(); i++)
        {
			min = std::min(this->ap_proc_time_per_level[i][t] - inactive_acumulators[i] - ap_processing_acumulator, min);
		}        

        return min;
    }
    
    // Running Scheduler
    void SlackStealingScheduler::start()
    {
		while(!periodic_arriving.empty()) periodic_arriving.pop();

        for (const Task& periodic : periodic_tasks)
        {
            periodic_arriving.push(periodic);
        }
        abs_time = 0;
		
		inactive_acumulators.resize(periodic_tasks.size());
    }

	void SlackStealingScheduler::resetAcumulators()
	{

		std::fill(this->inactive_acumulators.begin(), this->inactive_acumulators.end(), 0);
		ap_processing_acumulator = 0;
	}

	void SlackStealingScheduler::updateProcessingQueues()
	{   
		while(!this->periodic_arriving.empty() && this->periodic_arriving.top().arrival_time == this->abs_time)
        {
            Task task = this->periodic_arriving.top();
			this->periodic_arriving.pop();
			this->periodic_processing.push(task);
        }

		while(!this->aperiodic_arriving.empty() && this->aperiodic_arriving.top().arrival_time == this->abs_time)
        {
            Task task = this->aperiodic_arriving.top();
			this->aperiodic_arriving.pop();
			this->aperiodic_processing.push(task);
        }
	}

	Task SlackStealingScheduler::chooseTaskToProcess()
	{   
        if(this->FLAG_DEBUG)
            std::cout << "[i] chooseTaskToProcess()" << std::endl;
        
        Task task = this->periodic_processing.top();
		if(aperiodic_processing.empty())
        {   
            if(this->FLAG_DEBUG)    
                std::cout << "[i] | -- No aperiodic work ready." << std::endl;

			this->periodic_processing.pop();
            return task;
        }

		int slackTimeAvailable = getSlackTimeAvaiable(this->abs_time % this->H);

        if(this->FLAG_DEBUG)    
            std::cout << "[i] | -- Slack Time Available " << slackTimeAvailable << std::endl;

		bool enough_ap_processing = slackTimeAvailable > 0;
        if (!enough_ap_processing)
        {
			this->periodic_processing.pop();
			return task;
        }

		task = aperiodic_processing.top();
		this->aperiodic_processing.pop();
		return task;
	}
	
	void SlackStealingScheduler::updateAcumulators(const Task& task)
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

    void SlackStealingScheduler::updateAcumulators(unsigned cur_priority)
	{
        for(int i = cur_priority-1; i >= 0; i--)
        {
            inactive_acumulators[i] += 1;
        }
	}
	
	void SlackStealingScheduler::processTask(Task& task)
	{
		bool doneProcessing = task.compute();

		this->updateAcumulators(task);
        
		if(!doneProcessing)
        {
            if (task.isPeriodic())
				this->periodic_processing.push(task);
            else
                this->aperiodic_processing.push(task);
        }
		else
		{
			if(task.isPeriodic())
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

    void SlackStealingScheduler::tick()
    {
		// if at the beginning of a hyperperiod, reset acumulators
		if(this->abs_time % this->H == 0)
			this->resetAcumulators();
        
        std::cout << "[ ]" << std::endl;
        std::cout << "[ ]" << std::endl;
        std::cout << "[t] Time:" << this->abs_time << " | Relative time:" << this->abs_time % this->H << std::endl;

        // First step: add all ready tasks to processing queue
        this->updateProcessingQueues();

        if(this->FLAG_DEBUG)
        {
            std::cout << "[i] tick() debug info:" << std::endl;
            std::cout << "[i] | -- Periodic Arriving Size: " << this->periodic_arriving.size() << std::endl;
            std::cout << "[i] | -- Periodic Processing Size: " << this->periodic_processing.size() << std::endl;
            std::cout << "[i] | -- Aperiodic Arriving Size: " << this->aperiodic_arriving.size() << std::endl;
            std::cout << "[i] | -- Aperiodic Processing Size: " << this->aperiodic_processing.size() << std::endl;
        }  

        if (!this->aperiodic_processing.empty() || !this->periodic_processing.empty())
        {
            // Second step: choose task to process
            Task task_to_process = this->chooseTaskToProcess();

            // Third step: process task
            this->processTask(task_to_process);   

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
            unsigned cur_priority = this->periodic_tasks.size();
            this->updateAcumulators(cur_priority);

            std::cout << "[t] Processor Idle... ";
        }
        
        // Fourth step: advance time
        this->abs_time++;
    }

    void SlackStealingScheduler::addOnlineTask(Task t)
    {
        // Do nothing
    }
}