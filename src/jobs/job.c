//
// Created by wehuf on 9/26/2024.
//

#include "job.h"

void initJob(Job* job, int id, Job* deps, void (*execute)()) {
    job->id = id;
    pthread_mutex_init(&job->mutex, NULL);
    job->deps = deps;
    job->execute = execute;
    job->done = false;
}

void freeJob(Job* job) {
    pthread_mutex_destroy(&job->mutex);
    arrfree(job->deps);
}

void freeJobTree(Job* exit) {
    for (int i = 0; i < arrlen(exit->deps); ++i) {
        freeJobTree(&exit->deps[i]);
    }

    freeJob(exit);
}