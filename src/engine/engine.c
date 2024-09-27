//
// Created by wehuf on 9/26/2024.
//

#include "engine.h"

#include "../common.h"

#include "input.h"
#include "window.h"

static void printEngineInfo() {
    printf("GLFW version:   %s\n", windowGetGLFWVersionString());
    printf("GL version:     %s\n", (char*)glGetString(GL_VERSION));
    printf("GL renderer:    %s\n", (char*)glGetString(GL_RENDERER));
}

void initEngine() {
    initWindow();
    initInput();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    printEngineInfo();
}
void freeEngine() {
    freeInput();
    freeWindow();
}

void engineLoop() {
    while (!windowShouldClose()) {

        inputBeforePoll();
        windowPollEvents();
        inputAfterPoll();

        windowSwapBuffers();
    }
}
