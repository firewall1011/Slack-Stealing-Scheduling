#include "task.hpp"

namespace RTSTasks 
{
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

    // Ordering functions
    bool sortTaskByEarliestDeadline(const Task& a, const Task& b)
    {
        return (a.deadline != b.deadline) ? a.deadline < b.deadline : a.arrival_time <= b.arrival_time;
    }
    
    bool sortTaskByEarliestArrivalTime(const Task& a, const Task& b)
    {
        return a.arrival_time <= b.arrival_time;
    }

    bool sortTaskByLowPeriodTime(const Task& a, const Task& b)
    {   
        if( !a.isPeriodic() || !b.isPeriodic() )
            throw "Trying to order aperiodic tasks by period time";

        return a.period <= b.period;
    }
}