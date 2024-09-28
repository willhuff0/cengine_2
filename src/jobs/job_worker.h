//
// Created by wehuf on 9/26/2024.
//

#ifndef JOB_WORKER_H
#define JOB_WORKER_H

#include <pthread.h>

typedef struct {
    pthread_t thread;
} Worker;

void initWorker(Worker* worker, const char* name);
void freeWorker(const Worker* worker);

#endif //JOB_WORKER_H
