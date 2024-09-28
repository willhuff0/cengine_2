//
// Created by wehuf on 9/26/2024.
//

#include "job_system.h"

#include <stdlib.h>

#include "job_worker.h"

pthread_mutex_t jobSystemMutex;
bool jobSystemShouldShutdown;

static int numWorkers;
static Worker* workers;
static Job** trees;

void initJobSystem(int numThreads) {
    jobSystemMutex = pthread_mutex_init(&jobSystemMutex, NULL);
    pthread_mutex_lock(&jobSystemMutex);
    jobSystemShouldShutdown = false;

    numWorkers = numThreads;

    workers = malloc(sizeof(Worker) * numWorkers);
    for (int i = 0; i < numWorkers; i++) {
        char buffer[24];
        snprintf(buffer, 24, "Worker %d", i);
        initWorker(&workers[i], buffer);
    }

    trees = NULL;

    pthread_mutex_unlock(&jobSystemMutex);
}
void freeJobSystem() {
    pthread_mutex_lock(&jobSystemMutex);
    jobSystemShouldShutdown = true;
    pthread_mutex_unlock(&jobSystemMutex);

    for (int i = 0; i < numWorkers; ++i) {
        freeWorker(&workers[i]);
    }

    arrfree(trees);

    pthread_mutex_destroy(&jobSystemMutex);
}

void executeJobTreeAsync(const Job* exit) {
    arrput(trees, exit);
}

void executeJobTreeSync(const Job* exit) {
    executeJobTreeAsync(exit);
    waitForJobToFinish(exit);
}

// Returns NULL if job itself or all deps are in progress
static Job* findIncompleteJob(Job* job) {
    if (!tryLockJob(job)) return NULL; // job is already in progress
    if (job->done) return job;

    bool depInProgress = false;
    for (int i = 0; i < arrlen(job->deps); ++i) {
        Job* result = findIncompleteJob(&job->deps[i]);
        if (result == NULL) {
            // result mutex already unlocked
            depInProgress = true;
            continue;
        }
        if (result->done) {
            unlockJob(result);
            continue;
        }

        unlockJob(job);
        return result;
    }

    if (depInProgress) {
        unlockJob(job);
        return NULL;
    }

    return job;
}

// Gets the next job to execute and acquires a mutex lock. If there are no jobs to execute, returns NULL.
// Should only be called from worker threads.
Job* getNextJob() {
    if (trees == NULL) return NULL;

    for (int i = arrlen(trees) - 1; i >= 0; --i) {
        Job* next = findIncompleteJob(trees[i]);

        if (next == NULL) continue;

        if (next == trees[i] && next->done) {
            arrdel(trees, i);
            unlockJob(next);
            continue;
        }

        return next;
    }

    return NULL;
}

// TODO: Create separate job tree struct which workers will wait to lock when all jobs are done so that waitForJobToFinish doesnt fight over trylock