//
// Created by wehuf on 9/26/2024.
//

#include "job.h"

void initJob(Job* job, Job* deps, void (*execute)(JobData data), const JobData data, const char* name) {
    job->name = name;
    job->deps = deps;
    job->execute = execute;
    job->data = data;
    job->done = false;
    job->inProgress = false;
}

void freeJobAndDeps(Job* job) {
    for (int i = 0; i < arrlen(job->deps); ++i) {
        freeJobAndDeps(&job->deps[i]);
    }

    arrfree(job->deps);
}
static void resetJobAndDeps(Job* job) {
    for (int i = 0; i < arrlen(job->deps); ++i) {
        resetJobAndDeps(&job->deps[i]);
    }

    job->done = false;
    job->inProgress = false;
}

void initJobTree(JobTree* jobTree, Job* jobs, const char* name) {
    jobTree->name = name;
    jobTree->mutex = malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(jobTree->mutex, NULL);
    jobTree->jobs = jobs;
}
void freeJobTree(JobTree* jobTree) {
    pthread_mutex_destroy(jobTree->mutex);
    free(jobTree->mutex);

    for (int i = 0; i < arrlen(jobTree->jobs); ++i) {
        freeJobAndDeps(&jobTree->jobs[i]);
    }

    arrfree(jobTree->jobs);
}
void resetJobTree(JobTree* jobTree) {
    lockJobTree(jobTree);

    for (int i = 0; i < arrlen(jobTree->jobs); ++i) {
        resetJobAndDeps(&jobTree->jobs[i]);
    }

    jobTree->done = false;

    unlockJobTree(jobTree);
}

void waitForJobTreeToFinish(JobTree* jobTree) {
    while (true) {
        lockJobTree(jobTree);
        if (jobTree->done) {
            unlockJobTree(jobTree);
            break;
        }
        unlockJobTree(jobTree);
    }
}

void lockJobTree(JobTree* jobTree) {
    pthread_mutex_lock(jobTree->mutex);
    jobTree->locked = true;
}
void unlockJobTree(JobTree* jobTree) {
    jobTree->locked = false;
    pthread_mutex_unlock(jobTree->mutex);
}