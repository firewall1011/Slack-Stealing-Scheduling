#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "slack-stealing.h"

using namespace SlackStealing;

TEST_CASE("A* test"){
	int expected[13] {0, 1, 1, 1, 1, 1, 3, 3, 3, 3, 3, 3, 3}; 
	std::vector<Task> list_tasks;

	list_tasks.push_back(Task(1, 1, 0, 4, 1));
	list_tasks.push_back(Task(3, 6, 0, 6, 2));

	std::vector<int> A_star = aperiodic_free_time(12, list_tasks);

	REQUIRE(A_star.size() == 13);

	for (int i = 0; i < A_star.size(); i++)
	{
		CHECK_EQ(A_star[i], expected[i]);
	}
}