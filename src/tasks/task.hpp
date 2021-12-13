#pragma once

#include <iostream>

namespace RTSTasks 
{   
    class Task
    {
        public:
            // Commom tasks attributes ~ Paper notations
            unsigned computation_cost;   // ~ Ci (periodic)    | pk (aperiodic)
            unsigned arrival_time;       // ~ phi_i (periodic) | alpha_k (aperiodic)
            unsigned deadline;           // ~ Di (periodic)    | none
            unsigned finish_time;        // ~ Cij (periodic)   | Tk (aperiodic)
            unsigned response_time;      // ~ Cij (periodic)   | Rk (aperiodic)
            
            // Periodic specific functions
            unsigned period;            // Ti (periodic)
            unsigned job_instance;      //  j

            // Utilities Variables
            unsigned priority;           // ~ i (periodic)     | i (aperiodic)
            unsigned computed = 0;      // already computed time
            
            // Constructors
            Task(unsigned arrival_time, unsigned computation_cost, unsigned deadline, unsigned finish_time, unsigned response_time, unsigned period, unsigned job_instance);

            // Predicative Functions
            bool isPeriodic() const;

            bool isPeriodicInstance() const;
            
            bool isFinished() const;

            bool compute();

            // Default ordering: Earliest Arrival First
            bool operator<(const Task& other) const
            {
                return this->arrival_time < other.arrival_time;
            }
			
            // std::out function
			friend std::ostream& operator<<(std::ostream& os, const Task& t);
    };
}

// Aditional information:
// ----------------------
//
// - Difference between deadline, finish_time and response_time?
//   > deadline: limit time to a task be done or throws a time fault;
//   > finish_time: point in time when a task was finished;
//   > response_time: diff between finish_time - arrival_time;