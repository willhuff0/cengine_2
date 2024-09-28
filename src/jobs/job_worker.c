//
// Created by wehuf on 9/26/2024.
//

#include "job_worker.h"

#include "job_system.h"
#include "../common.h"

static void workerFunc(char* name) {
    while(true) {
        pthread_mutex_lock(&jobSystemMutex);

        if (jobSystemShouldShutdown) {
            pthread_mutex_unlock(&jobSystemMutex);
            break;
        }

        const GetNextJobResult getNextJobResult = getNextJob();
        pthread_mutex_unlock(&jobSystemMutex);

        if (getNextJobResult.job == NULL) continue;

#ifdef JOB_SYSTEM_DEBUG_PRINT
        printf("Worker '%s' <= S job '[%s] %s'\n", name, getNextJobResult.jobTree->name, getNextJobResult.job->name);
#endif

        getNextJobResult.job->execute(getNextJobResult.job->data);

        lockJobTree(getNextJobResult.jobTree);

        getNextJobResult.job->inProgress = false;
        getNextJobResult.job->done = true;

        bool done = true;
        for (int i = 0; i < arrlen(getNextJobResult.jobTree->jobs); ++i) {
            if (!getNextJobResult.jobTree->jobs[i].done) done = false;
        }
        if (done) {
            getNextJobResult.jobTree->done = true;
        }

        unlockJobTree(getNextJobResult.jobTree);

#ifdef JOB_SYSTEM_DEBUG_PRINT
        printf("Worker '%s' F => job '[%s] %s'\n", name, getNextJobResult.jobTree->name, getNextJobResult.job->name);
#endif
    }

    free(name);
}

void initWorker(Worker* worker, char* name) {
    pthread_create(&worker->thread, NULL, workerFunc, name);
}

void freeWorker(const Worker* worker) {
    pthread_join(worker->thread, NULL);
}