//
// Created by Will on 7/6/24.
//

#include "fps_counter.h"

#include <math.h>
#include <stdio.h>
#include "../../common.h"

#define ENABLE_FPS_COUNTER
#define CAPTURE_DURATION 2.0

static double captureStartTime;
static int frames;

void startFpsCounter() {
#ifdef ENABLE_FPS_COUNTER
    captureStartTime = glfwGetTime();
    frames = 0;
#endif
}
void tickFpsCounter() {
#ifdef ENABLE_FPS_COUNTER
    frames++;

    const double currentTime = glfwGetTime();
    if (currentTime - captureStartTime >= CAPTURE_DURATION) {
        printf("%d FPS (t=%d)\n", (int)round(frames / CAPTURE_DURATION), (int)currentTime);

        captureStartTime = currentTime;
        frames = 0;
    }
#endif
}