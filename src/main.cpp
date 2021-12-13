#include <iostream>
#include <unistd.h>
#include "tasks/task.hpp"
#include "tasks/taskbuilder.hpp"
#include "schedulers/rateMonotonic.hpp"

using namespace RTSTasks;
using namespace RTSSCheduler;

int main(void){

    try 
    { 
        RateMonotonicScheduler scheduler;

        scheduler.preloadTask(TaskBuilder::createPeriodicTask(0, 3, 1, 1));
        scheduler.preloadTask(TaskBuilder::createPeriodicTask(0, 6, 1, 3));
        scheduler.preloadTask(TaskBuilder::createTask(3, 4));

        scheduler.prepareScheduler();

        scheduler.start();

        std::cout << "Starting scheduler..." << std::endl;

        while(true)
        {
            scheduler.tick();
            std::cin.get();
        }

    } 
    catch(const std::exception& ex) 
    { 
        std::cerr << ex.what() << std::endl;
    }

    return EXIT_SUCCESS;
}