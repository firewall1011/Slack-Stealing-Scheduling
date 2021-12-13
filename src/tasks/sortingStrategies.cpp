#include "sortingStrategies.hpp"

namespace RTSTasks
{
    bool sortTaskByPriority(const Task& a, const Task& b)
    {
        return a.priority < b.priority;
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

    // Used by EDS Algorithm
    bool sortTaskByEarliestDeadline(const Task& a, const Task& b)
    {
        return (a.deadline != b.deadline) ? a.deadline < b.deadline : a.arrival_time <= b.arrival_time;
    }

    // Used by RMS Algorithm
    bool sortTaskByRateMonotonic(const Task& a, const Task& b)
    {   
        if( !a.isPeriodic() || !b.isPeriodic() )
            throw "Trying to order aperiodic tasks by period time";
        return (a.deadline/double(a.period)) < (b.deadline/double(b.period));
    }
}