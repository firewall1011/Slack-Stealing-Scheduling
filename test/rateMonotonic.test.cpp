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

	SUBCASE("A*(t) calculation") {
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

	SUBCASE("Ai(t) calculation") {
		unsigned expected_A[13] {0, 3, 3, 3, 3, 6, 6, 6, 6, 9, 9, 9, 9}; 
		unsigned expected_B[13] {1, 1, 1, 1, 1, 1, 3, 3, 3, 3, 3, 3, 3}; 
		
		RateMonotonicScheduler scheduler;
		scheduler.preloadTask(taskA);
		scheduler.preloadTask(taskB);
		scheduler.prepareScheduler();
		
		REQUIRE(scheduler.ap_proc_time_per_level[0].size() == sizeof(expected_A) / sizeof(unsigned));
		REQUIRE(scheduler.ap_proc_time_per_level[1].size() == sizeof(expected_B) / sizeof(unsigned));
		
		auto& ap_proc_time = scheduler.ap_proc_time_per_level[0];
		for(int i = 0; i < ap_proc_time.size(); i++)
		{
			CHECK(ap_proc_time[i] == expected_A[i]);
		}
		
		ap_proc_time = scheduler.ap_proc_time_per_level[1];
		for(int i = 0; i < ap_proc_time.size(); i++)
		{
			CHECK(ap_proc_time[i] == expected_B[i]);
		}
	}
}