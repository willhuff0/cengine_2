//
// Created by wehuf on 9/26/2024.
//

#ifndef JOB_H
#define JOB_H

#include "../common.h"

#include <pthread.h>

typedef struct Job Job;

typedef struct JobDeps {
    int numDeps;
    Job* deps;
    Job* dynamicDeps;
} JobDeps;

typedef union {
    void* ptr;
    int64_t number;
    uint64_t unsignedNumber;
    struct {
        int n1;
        int n2;
    } twoNumbers;
} JobData;

struct Job {
    const char* name;
    JobDeps deps;
    void (*execute)(JobData data);
    JobData data;

    // Should only be set if parent jobTree is mutex locked
    bool done;
    bool inProgress;
};

typedef struct {
    const char* name;
    pthread_mutex_t* mutex;
    int numJobs;
    Job* jobs;
    bool done;

    // Debug
    bool locked;
} JobTree;

void initJob(Job* job, const int numDeps, Job* deps, void (*execute)(JobData data), const JobData data, const char* name);
void freeJobAndDeps(Job* job);

void clearJobDynamicDeps(Job* job);

void initJobTree(JobTree* jobTree, const int numJobs, Job* jobs, const char* name);
void freeJobTree(const JobTree* jobTree);
// Locks job tree for the caller
void resetJobTree(JobTree* jobTree);

void waitForJobTreeToFinish(JobTree* jobTree);

void lockJobTree(JobTree* jobTree);
void unlockJobTree(JobTree* jobTree);

#endif //JOB_H
