//
// Created by wehuf on 9/26/2024.
//

#ifndef JOB_H
#define JOB_H

#include "../common.h"

#include <pthread.h>

typedef struct Job Job;

struct Job {
    const char* name;
    pthread_mutex_t* mutex;
    Job* deps;
    void (*execute)();
    bool done;
};

void initJob(Job* job, Job* deps, void (*execute)(), const char* name);
void freeJob(Job* job);

void freeJobTree(Job* exit);

void resetJobTree(Job* exit);

void waitForJobToFinish(const Job* job);

#endif //JOB_H
