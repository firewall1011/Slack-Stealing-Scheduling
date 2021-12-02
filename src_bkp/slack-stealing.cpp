#include "slack-stealing.h"
#include <algorithm>

namespace SlackStealing
{
	bool Task::operator < (const Task& t) const
	{
		return this->priority < t.priority;
	}

	bool Task::operator > (const Task& t) const
	{
		return this->priority > t.priority;
	}

	int freq(Task task, int t)
	{
		int max = std::max<int>(0, t - task.arrival_time);
		int period = (max / task.period) + (max % task.period != 0);

		return period;
	}

	int periodic_ready_work(int t, int priority, std::vector<Task> task)
	{
		int sum = 0;
		if (priority != 1)
		{
			for (int j = 1; j <= priority; j++)
			{
				sum += (task[j-1].exec_time * freq(task[j-1], t));
			}
		}
		else
		{
			sum = task[0].exec_time * freq(task[0], t);
		}

		return sum;
	}

	void compute_min_free_time(const int t, std::vector<int>& a_star, std::vector<int*> slots)
	{
		a_star.reserve(t+1);
	
		for (int h = 0; h < t+1; h++)
		{
			int min = std::numeric_limits<int>::max();
			for (int g = 0; g < slots.size(); g++)
			{
				min = std::min(slots[g][h], min);
			}
			a_star.emplace_back(min);
		}
	}

	int* ap_time_of_priority(int t, int priority, std::vector<Task> tasks)
	{
		int* ap_times = new int[t+1];

		int last_at = -1;
		int idx = priority-1;

		for (int deadline = tasks[idx].dead_line; deadline <= t; deadline = std::min(deadline + tasks[idx].period, t))
		{
			int ap_time = deadline - periodic_ready_work(deadline, priority, tasks);

			for (int h = last_at+1; h <= deadline; h++)
			{
				ap_times[h] = ap_time;
			}
			last_at = deadline-1;

			if(deadline == t) break;
		}

		return ap_times;
	}

	std::vector<int> aperiodic_free_time(const int t, std::vector<Task> task_p)
	{
		sort(task_p.begin(), task_p.end());

		std::vector<int*> slots(task_p.size());

		for (int task_id = 0; task_id < task_p.size(); task_id++)
		{
			slots[task_id] = ap_time_of_priority(t, task_id + 1, task_p);
		}

		std::vector<int> a_star;
		compute_min_free_time(t, a_star, slots);

		for(int* slot : slots)
		{
			delete[] slot;
		}

		return a_star;
	}
}