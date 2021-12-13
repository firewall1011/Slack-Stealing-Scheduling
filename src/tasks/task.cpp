#include "task.hpp"

namespace RTSTasks 
{
    // Default Constructor
    Task::Task(unsigned arrival_time, unsigned computation_cost, unsigned deadline, unsigned finish_time, unsigned response_time, unsigned period, unsigned job_instance):
        arrival_time(arrival_time),
        computation_cost(computation_cost),
        deadline(deadline),
        finish_time(finish_time),
        response_time(response_time),
        period(period),
        job_instance(job_instance) { }

    // Predicative Functions
    bool Task::isPeriodic() const
    { 
        return this->period > 0;
    }

    bool Task::isPeriodicInstance() const
    {
        return this->isPeriodic() && this->job_instance > 0; 
    }
    
    bool Task::isFinished() const
    {
        return this->finish_time > 0;
    }

    bool Task::compute()
    {
        computed++;
        return (computed >= computation_cost);
    }

    // STD Output Stream Functions
	std::ostream& printPeriodicTask(std::ostream& os, const Task& t)
	{
		return os << "Task:" << t.job_instance
			<< " priority:" << t.priority
			<< " computed:" << t.computed << "/" << t.computation_cost
			<< " period:" << t.period
			<< " arrival:" << t.arrival_time
			<< " deadline:" << t.deadline
			<< " is Periodic";
	}
	
	std::ostream& printAperiodicTask(std::ostream& os, const Task& t)
	{
		return os << "Task:"
			<< " computed:" << t.computed << "/" << t.computation_cost
			<< " arrival:" << t.arrival_time
			<< " is Aperiodic";
	}

    // Default ordering: Earliest Arrival First
	std::ostream& operator<<(std::ostream& os, const Task& t)
	{
		return t.isPeriodic() ? printPeriodicTask(os, t) : printAperiodicTask(os, t);
	}
}