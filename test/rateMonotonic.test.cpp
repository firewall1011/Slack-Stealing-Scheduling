#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "schedulers/rateMonotonic.hpp"
#include "tasks/taskbuilder.hpp"

using namespace RTSSCheduler;
using namespace RTSTasks;

TEST_CASE("RateMonotonicScheduler test") {
	
	auto taskA = TaskBuilder::createPeriodicTask(0, 4, 1, 1);
	auto taskB = TaskBuilder::createPeriodicTask(0, 6, 3, 6);
	
	SUBCASE("Preload feasible set of tasks doesn't throw exception") {
		RateMonotonicScheduler scheduler;
		scheduler.preloadTask(taskA);
		scheduler.preloadTask(taskB);

		CHECK_NOTHROW(scheduler.prepareScheduler());
	}

	SUBCASE("Preload infeasible set of tasks throws exception") {
		RateMonotonicScheduler scheduler;
		scheduler.preloadTask(taskA);
		scheduler.preloadTask(taskB);
		scheduler.preloadTask(taskB);
		scheduler.preloadTask(taskA);
		scheduler.preloadTask(taskA);

		CHECK_THROWS_AS(scheduler.prepareScheduler(), const std::runtime_error&);
	}

	SUBCASE("A* calculation") {
		unsigned expected[13] {0, 1, 1, 1, 1, 1, 3, 3, 3, 3, 3, 3, 3}; 
		
		RateMonotonicScheduler scheduler;
		scheduler.preloadTask(taskA);
		scheduler.preloadTask(taskB);
		scheduler.prepareScheduler();
		
		REQUIRE(scheduler.ap_proc_time_zero_H.size() == sizeof(expected) / sizeof(unsigned));
		
		for(int i = 0; i < scheduler.ap_proc_time_zero_H.size(); i++)
		{
			CHECK(scheduler.ap_proc_time_zero_H[i] == expected[i]);
		}
	}
}