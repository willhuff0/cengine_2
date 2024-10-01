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

// Must have jobSystemMutex lock
// Should only be called from worker threads.
void deleteJobTree(const JobTree* jobTree);

// Must have jobSystemMutex lock
// Gets the next job to execute and sets inProgress to true. If there are no jobs to execute, returns NULL.
// Should only be called from worker threads.
GetNextJobResult getNextJob();


#endif //JOB_SYSTEM_H
