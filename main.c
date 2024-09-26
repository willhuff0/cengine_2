#include <stdio.h>
#include <unistd.h>

#include "src/jobs/jobs.h"

#define SLEEP_DURATION 5

void exec1() {
    printf("Job 1 starting...\n");
    sleep(SLEEP_DURATION);
    printf("Job 1 done.\n");
}

void exec11() {
    printf("Job 11 starting...\n");
    sleep(SLEEP_DURATION);
    printf("Job 11 done.\n");
}

void exec12() {
    printf("Job 12 starting...\n");
    sleep(SLEEP_DURATION);
    printf("Job 12 done.\n");
}

void exec13() {
    printf("Job 13 starting...\n");
    sleep(SLEEP_DURATION);
    printf("Job 13 done.\n");
}

void exec111() {
    printf("Job 111 starting...\n");
    sleep(SLEEP_DURATION);
    printf("Job 111 done.\n");
}

void exec112() {
    printf("Job 112 starting...\n");
    sleep(SLEEP_DURATION);
    printf("Job 112 done.\n");
}

int main(void)
{
    initJobSystem(8);

    Job job13;
    initJob(&job13, 13, NULL, exec13);
    Job job12;
    initJob(&job12, 12, NULL, exec12);

    Job job111;
    initJob(&job111, 111, NULL, exec111);
    Job job112;
    initJob(&job112, 112, NULL, exec112);

    Job* job11deps = NULL;
    arrput(job11deps, job111);
    arrput(job11deps, job112);

    Job job11;
    initJob(&job11, 11, job11deps, exec11);

    Job* job1deps = NULL;
    arrput(job1deps, job11);
    arrput(job1deps, job12);
    arrput(job1deps, job13);

    Job job1;
    initJob(&job1, 1, job1deps, exec1);

    pthread_mutex_lock(&jobSystemMutex);
    addJobTree(&job1);
    pthread_mutex_unlock(&jobSystemMutex);

    while (true) {
        pthread_mutex_lock(&job1.mutex);
        if (job1.done) break;
        pthread_mutex_unlock(&job1.mutex);
        sleep(1);
    }

    freeJobSystem();
    freeJobTree(&job1);

    return 0;
}
