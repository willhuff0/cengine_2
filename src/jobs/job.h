//
// Created by wehuf on 9/26/2024.
//

#ifndef JOB_H
#define JOB_H

#include "../common.h"

#include <pthread.h>

typedef struct Job Job;

struct Job {
    int id;
    pthread_mutex_t mutex;
    Job* deps;
    void (*execute)();
    bool done;
};

void initJob(Job* job, int id, Job* deps, void (*execute)());
void freeJob(Job* job);

void freeJobTree(Job* exit);

#endif //JOB_H
