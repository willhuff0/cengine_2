//
// Created by wehuf on 9/26/2024.
//

#ifndef JOB_H
#define JOB_H

#include "../common.h"

#include <pthread.h>

typedef struct Job Job;

typedef union {
    void* ptr;
    int64_t number;
    uint64_t unsignedNumber;
    struct {
        int n1;
        int n2;
    } twoNumbers;
} JobData;

struct Job {
    const char* name;
    pthread_mutex_t* mutex;
    Job* deps;
    void (*execute)(JobData data);
    JobData data;
    bool done;
    bool locked;
};

void initJob(Job* job, Job* deps, void (*execute)(JobData data), const JobData data, const char* name);
void freeJob(Job* job);

void freeJobTree(Job* exit);

void resetJobTree(Job* exit);

void waitForJobToFinish(const Job* job);

void lockJob(Job* job);
bool tryLockJob(Job* job);
void unlockJob(Job* job);

#endif //JOB_H
