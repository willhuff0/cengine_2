//
// Created by wehuf on 9/26/2024.
//

#include "job.h"

void initJob(Job* job, const int numDeps, Job* deps, void (*execute)(JobData data), const JobData data, const char* name) {
    job->name = name;

    job->deps.numDeps = numDeps;
    job->deps.deps = deps;
    job->deps.dynamicDeps = NULL;

    job->execute = execute;
    job->data = data;
    job->done = false;
    job->inProgress = false;
}

void freeJobAndDeps(Job* job) {
    for (int i = 0; i < job->deps.numDeps; ++i) {
        freeJobAndDeps(&job->deps.deps[i]);
    }
    free(job->deps.deps);

    for (int i = 0; i < arrlen(job->deps.dynamicDeps); ++i) {
        freeJobAndDeps(&job->deps.dynamicDeps[i]);
    }
    arrfree(job->deps.dynamicDeps);
}
static void resetJobAndDeps(Job* job) {
    for (int i = 0; i < job->deps.numDeps; ++i) {
        resetJobAndDeps(&job->deps.deps[i]);
    }

    for (int i = 0; i < arrlen(job->deps.dynamicDeps); ++i) {
        resetJobAndDeps(&job->deps.dynamicDeps[i]);
    }

    job->done = false;
    job->inProgress = false;
}

void clearJobDynamicDeps(Job* job) {
    for (int i = 0; i < arrlen(job->deps.dynamicDeps); ++i) {
        clearJobDynamicDeps(&job->deps.dynamicDeps[i]);
    }
    arrfree(job->deps.dynamicDeps);
    job->deps.dynamicDeps = NULL;
}

void initJobTree(JobTree* jobTree, const int numJobs, Job* jobs, const char* name) {
    jobTree->name = name;
    jobTree->mutex = malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(jobTree->mutex, NULL);
    jobTree->numJobs = numJobs;
    jobTree->jobs = jobs;
}
void freeJobTree(const JobTree* jobTree) {
    pthread_mutex_destroy(jobTree->mutex);
    free(jobTree->mutex);

    for (int i = 0; i < jobTree->numJobs; ++i) {
        freeJobAndDeps(&jobTree->jobs[i]);
    }
    free(jobTree->jobs);
}
void resetJobTree(JobTree* jobTree) {
    lockJobTree(jobTree);

    for (int i = 0; i < jobTree->numJobs; ++i) {
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