//
// Created by wehuf on 9/26/2024.
//

#ifndef JOB_SYSTEM_H
#define JOB_SYSTEM_H

#include "job.h"

//#define JOB_SYSTEM_DEBUG_PRINT

typedef struct {
    Job* job;
    JobTree* jobTree;
} GetNextJobResult;

extern pthread_mutex_t jobSystemMutex;
extern bool jobSystemShouldShutdown;

void initJobSystem(const int numThreads);
void freeJobSystem();

void executeJobTreeAsync(const JobTree* jobTree);
void executeJobTreeSync(JobTree* jobTree);

// Gets the next job to execute and acquires a mutex lock. If there are no jobs to execute, returns nullptr.
// Should only be called from worker threads.
GetNextJobResult getNextJob();


#endif //JOB_SYSTEM_H
