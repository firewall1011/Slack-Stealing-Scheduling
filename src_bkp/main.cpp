#include "slack-stealing.h"
#include <iostream>

int main()
{
	using namespace std;
	using namespace SlackStealing;
	
	vector<Task> list_tasks;

	list_tasks.push_back(Task(1, 1, 0, 4, 1));
	list_tasks.push_back(Task(3, 6, 0, 6, 2));

	vector<int> A_star = aperiodic_free_time(12, list_tasks);

	cout << "\n\n===== MAIN ====\n\n";
	cout << "A*: ";
	for (int i = 0; i < A_star.size(); i++)
	{
		cout << A_star[i] << " ";
	}
	cout << "\n\n";
}