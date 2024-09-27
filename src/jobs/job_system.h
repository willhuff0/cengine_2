//
// Created by wehuf on 9/26/2024.
//

#ifndef JOB_SYSTEM_H
#define JOB_SYSTEM_H

#include "job.h"

#define JOB_SYSTEM_DEBUG_PRINT

extern pthread_mutex_t jobSystemMutex;
extern bool jobSystemShouldShutdown;

void initJobSystem(int numThreads);
void freeJobSystem();

void pushJobTree(const Job* exit);
void executeJobTree(const Job* exit);

// Gets the next job to execute and acquires a mutex lock. If there are no jobs to execute, returns nullptr.
// Should only be called from worker threads.
Job* getNextJob();


#endif //JOB_SYSTEM_H
