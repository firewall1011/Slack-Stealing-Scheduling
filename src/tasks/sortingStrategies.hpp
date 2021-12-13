#pragma once
#include "tasks/task.hpp"

namespace RTSTasks
{
    bool sortTaskByPriority(const Task& a, const Task& b);

    bool sortTaskByEarliestArrivalTime(const Task& a, const Task& b);

    bool sortTaskByLowPeriodTime(const Task& a, const Task& b);

    // Used by EDS Algorithm
    bool sortTaskByEarliestDeadline(const Task& a, const Task& b);

    // Used by RMS Algorithm
    bool sortTaskByRateMonotonic(const Task& a, const Task& b);
}