//
// Created by wehuf on 9/26/2024.
//

#include "engine.h"

#include "draw_system.h"
#include "../common.h"

#include "input.h"
#include "window.h"
#include "../renderer/frame_packet.h"
#include "../renderer/renderer.h"
#include "../simulation/sim.h"
#include "utils/fps_counter.h"

static void printEngineInfo() {
    printf("GLFW version:   %s\n", windowGetGLFWVersionString());
    printf("GL version:     %s\n", (char*)glGetString(GL_VERSION));
    printf("GL renderer:    %s\n", (char*)glGetString(GL_RENDERER));
}

void initEngine() {
    initWindow();
    initInput();
    initFramePackets();
    initDrawSystem();
    initAssets();

    initSim();
    initRenderer();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    printEngineInfo();
}
void freeEngine() {
    freeRenderer();
    freeSim();

    freeAssets();
    freeDrawSystem();
    freeFramePackets();
    freeInput();
    freeWindow();
}

void engineLoop() {
    startFpsCounter();

    while (!windowShouldClose()) {
        // Render previous frame packet (if available)
        // Run renderer in parallel
        executeRenderTreeAsync();

        // Poll Events
        inputBeforePoll();
        windowPollEvents();

        // Run simulation in parallel
        executeSimTreeAsync();

        // Wait for renderer to finish
        waitForJobTreeToFinish(&renderTree);

        // Execute draw queue
        executeDrawQueue();

        // Present drawn frame
        windowSwapBuffers();

        // Wait for simulation to finish
        waitForJobTreeToFinish(&simTree);

        // Swap frame packets
        swapFramePackets();

        // Clock FPS
        tickFpsCounter();
    }
}
