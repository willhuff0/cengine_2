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

void pushJobTree(const Job* exit) {
    arrput(trees, exit);
}

void executeJobTree(const Job* exit) {
    pushJobTree(exit);
    while (true) {
        pthread_mutex_lock(exit->mutex);
        if (exit->done) break;
        pthread_mutex_unlock(exit->mutex);
    }
}

// Returns nullptr if job itself or all deps are in progress
static Job* findIncompleteJob(Job* job) {
    if (pthread_mutex_trylock(job->mutex) != 0) return NULL; // job is already in progress
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
            pthread_mutex_unlock(result->mutex);
            continue;
        }

        pthread_mutex_unlock(job->mutex);
        return result;
    }

    if (depInProgress) {
        pthread_mutex_unlock(job->mutex);
        return NULL;
    }

    return job;
}

// Gets the next job to execute and acquires a mutex lock. If there are no jobs to execute, returns nullptr.
// Should only be called from worker threads.
Job* getNextJob() {
    if (trees == NULL) return NULL;

    // Delete all job trees from list that are done
    for (int i = arrlen(trees) - 1; i >= 0; --i) {
        if (trees[i]->done) {
            arrdel(trees, i);
        }
    }

    for (int i = 0; i < arrlen(trees); i++) {
        Job* next = findIncompleteJob(trees[i]);
        if (next != NULL) return next;
    }

    return NULL;
}