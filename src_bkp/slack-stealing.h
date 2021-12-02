#pragma once
#include <vector>

namespace SlackStealing
{
	class Task
	{
		public:
			int exec_time;
			int dead_line;
			int arrival_time;
			int period;
			int priority;
			
			Task(int exec_time, int dead_line, int arrival_time, int period, int priority) : 
			exec_time(exec_time), dead_line(dead_line), 
			arrival_time(arrival_time), period(period), priority(priority) { }

			bool operator < (const Task& t) const;
			bool operator > (const Task& t) const;
	};

	int freq(Task task, int t);
	
	int periodic_ready_work(int t, int priority, std::vector<Task> task);
	
	void compute_min_free_time(const int t, std::vector<int>& a_star, std::vector<int*> slots);
	
	int* ap_time_of_priority(int t, int priority, std::vector<Task> tasks);

	std::vector<int> aperiodic_free_time(const int t, std::vector<Task> task_p);
}