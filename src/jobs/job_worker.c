//
// Created by wehuf on 9/26/2024.
//

#include "job_worker.h"

#include "job_system.h"
#include "../common.h"

static void workerFunc() {
    while(true) {
        pthread_mutex_lock(&jobSystemMutex);

        if (jobSystemShouldShutdown) {
            pthread_mutex_unlock(&jobSystemMutex);
            break;
        }

        Job* job = getNextJob();
        pthread_mutex_unlock(&jobSystemMutex);

        if (job == NULL) continue;

        job->execute();
        job->done = true;
        pthread_mutex_unlock(&job->mutex);
    }
}

void initWorker(Worker* worker) {
    pthread_create(&worker->thread, NULL, workerFunc, NULL);
}

void freeWorker(Worker* worker) {
    pthread_join(worker->thread, NULL);
}