#include <iostream>
#include <unistd.h>
#include <string>
#include "utils/outputs.hpp"
#include "tasks/task.hpp"
#include "tasks/taskbuilder.hpp"
#include "schedulers/rateMonotonic.hpp"
#include "schedulers/slackStealing.hpp"

using namespace RTSTasks;
using namespace RTSSCheduler;

int main(void){

    // User Inputs
    int userOpt = -1, taskArrival, taskCost, taskPeriod, taskDeadline;
    std::string taskName;

    // Variables
    Task auxTask = Task(0,0,0,0,0,0,0);
    BaseScheduler* scheduler = nullptr;
    
    // Program Pré- Configurations
    printHeader();

    // 1 ~ Selecting the Scheduler
    printf("[!] Select the Scheduler: [0] Rate Monotonic | [1] Slack Stealing: ");
    std::cin >> userOpt;

    if(userOpt == 0)
        scheduler = new RateMonotonicScheduler();
    else if (userOpt == 1)
        scheduler = new SlackStealingScheduler();    
    else
    {
        printf("[x] Error: Invalid Scheduler Selection! Exited!");
        return EXIT_FAILURE;
    }

    // 2 ~ Preloading Tasks
    userOpt = -1;
    while(userOpt != 0)
    {
        printf("[ ]\n");
        printf("[!] Create New Task: [0] Done | [1] Periodic | [2] Aperiodic: ");
        std::cin >> userOpt;

        if(userOpt == 0)
            break;
        
        else if(userOpt == 1)
        {   
            try
            {   
                printf("[!] <Name, Cost, Period, Deadline>: "); 
                std::cin >> taskName >> taskCost >> taskPeriod >> taskDeadline;
                auxTask = TaskBuilder::createPeriodicTask(0, taskCost, taskPeriod, taskDeadline);
                auxTask.name = taskName;
                scheduler->preloadTask(auxTask);
                std::cout << "[+] Task " << taskName << " preloaded!" << std::endl;
            }
            catch(const std::exception& e)
            {
                std::cout << "[x] Error in " << e.what() << std::endl;
            }
        }

        else if(userOpt == 2)
        {
            try
            {   
                printf("[!] <Name, Cost, ArrivalTime>: ");
                std::cin >> taskName >> taskCost >> taskArrival;
                auxTask = TaskBuilder::createTask(taskArrival, taskCost);
                auxTask.name = taskName;
                scheduler->preloadTask(auxTask);
                std::cout << "[+] Task " << taskName << " preloaded!" << std::endl;
            }
            catch(const std::exception& e)
            {
                std::cout << "[x] Error in " << e.what() << std::endl;
            }
        }

        else
        {
            printf("[x] Error: Invalid Task Option! Exited!");
            return EXIT_FAILURE;
        }
    }
    
    // 3 ~ Running the scheduler
    try
    {   
        printf("[ ]\n");
        printf("[+] Preparing the scheduler...\n");
        scheduler->prepareScheduler();

        printf("[+] Starting the scheduler...\n");
        scheduler->start();

        printf("[+] Scheduler started: [ENTER] to tick() | [EOF] to quit!\n");
        printf("[ ]\n");

        while(std::cin.get() != EOF)
        {
            scheduler->tick();
        }
    }
    catch(const std::exception& e)
    {
        std::cout << "[x] Error in " << e.what() << std::endl;
    }

    return EXIT_SUCCESS;
}