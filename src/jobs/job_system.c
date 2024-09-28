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
static JobTree** jobTrees;

void initJobSystem(const int numThreads) {
    jobSystemMutex = pthread_mutex_init(&jobSystemMutex, NULL);
    pthread_mutex_lock(&jobSystemMutex);
    jobSystemShouldShutdown = false;

    numWorkers = numThreads;

    workers = malloc(sizeof(Worker) * numWorkers);
    for (int i = 0; i < numWorkers; i++) {
        char* buffer = malloc(24 * sizeof(char));
        snprintf(buffer, 24, "%d", i);
        initWorker(&workers[i], buffer);
    }

    jobTrees = NULL;

    pthread_mutex_unlock(&jobSystemMutex);
}
void freeJobSystem() {
    pthread_mutex_lock(&jobSystemMutex);
    jobSystemShouldShutdown = true;
    pthread_mutex_unlock(&jobSystemMutex);

    for (int i = 0; i < numWorkers; ++i) {
        freeWorker(&workers[i]);
    }

    for (int i = 0; i < arrlen(jobTrees); ++i) {
        freeJobTree(jobTrees[i]);
    }
    arrfree(jobTrees);

    pthread_mutex_destroy(&jobSystemMutex);
}

void executeJobTreeAsync(const JobTree* jobTree) {
    pthread_mutex_lock(&jobSystemMutex);
    arrput(jobTrees, jobTree);
    pthread_mutex_unlock(&jobSystemMutex);
}

void executeJobTreeSync(JobTree* jobTree) {
    executeJobTreeAsync(jobTree);
    waitForJobTreeToFinish(jobTree);
}

// Returns NULL if job itself or all deps are in progress
static Job* findIncompleteJob(Job* job) {
    if (job->inProgress) return NULL;
    if (job->done) return job;

    bool depInProgress = false;
    for (int i = 0; i < arrlen(job->deps); ++i) {
        Job* result = findIncompleteJob(&job->deps[i]);

        if (result == NULL) {
            depInProgress = true;
            continue;
        }
        if (result->done) continue;

        return result;
    }

    if (depInProgress) return NULL;

    return job;
}

// Gets the next job to execute and acquires a mutex lock. If there are no jobs to execute, returns NULL.
// Should only be called from worker threads.
GetNextJobResult getNextJob() {
    if (jobTrees == NULL) return (GetNextJobResult){ NULL, NULL };

    for (int i = arrlen(jobTrees) - 1; i >= 0; --i) {
        JobTree* jobTree = jobTrees[i];

        lockJobTree(jobTree);
        if (jobTree->done) {
            arrdel(jobTrees, i);
            unlockJobTree(jobTree);
            continue;
        }

        for (int k = 0; k < arrlen(jobTree->jobs); ++k) {
            Job* next = findIncompleteJob(&jobTree->jobs[k]);

            if (next == NULL) continue;
            if (next->done) continue;

            next->inProgress = true;
            unlockJobTree(jobTree);
            return (GetNextJobResult){ next, jobTree };
        }

        unlockJobTree(jobTree);
    }

    return (GetNextJobResult){ NULL, NULL };
}
