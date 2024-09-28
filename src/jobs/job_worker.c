//
// Created by wehuf on 9/26/2024.
//

#include "job_worker.h"

#include "job_system.h"
#include "../common.h"

static void workerFunc(const char* name) {
    while(true) {
        pthread_mutex_lock(&jobSystemMutex);

        if (jobSystemShouldShutdown) {
            pthread_mutex_unlock(&jobSystemMutex);
            break;
        }

        Job* job = getNextJob();
        pthread_mutex_unlock(&jobSystemMutex);

        if (job == NULL) continue;

#ifdef JOB_SYSTEM_DEBUG_PRINT
        printf("Worker '%s' starting job '%s'", name, job->name);
#endif

        job->execute(job->data);
        job->done = true;
        pthread_mutex_unlock(job->mutex);

#ifdef JOB_SYSTEM_DEBUG_PRINT
        printf("Worker '%s' finished job '%s'", name, job->name);
#endif
    }
}

void initWorker(Worker* worker, const char* name) {
    pthread_create(&worker->thread, NULL, workerFunc, (void*)name);
}

void freeWorker(const Worker* worker) {
    pthread_join(worker->thread, NULL);
}