//
// Created by wehuf on 9/26/2024.
//

#include "job.h"

void initJob(Job* job, Job* deps, void (*execute)(), const char* name) {
    job->name = name;
    job->mutex = malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(job->mutex, NULL);
    job->deps = deps;
    job->execute = execute;
    job->done = false;
}

void freeJob(Job* job) {
    pthread_mutex_destroy(job->mutex);
    free(job->mutex);
    arrfree(job->deps);
}

void freeJobTree(Job* exit) {
    for (int i = 0; i < arrlen(exit->deps); ++i) {
        freeJobTree(&exit->deps[i]);
    }

    freeJob(exit);
}

void resetJobTree(Job* exit) {
    for (int i = 0; i < arrlen(exit->deps); ++i) {
        resetJobTree(&exit->deps[i]);
    }

    exit->done = false;
}

void waitForJobToFinish(const Job* job) {
    while (true) {
        pthread_mutex_lock(job->mutex);
        if (job->done) break;
        pthread_mutex_unlock(job->mutex);
    }
}