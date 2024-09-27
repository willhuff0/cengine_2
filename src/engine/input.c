//
// Created by wehuf on 9/26/2024.
//

#include "input.h"

#include <pthread.h>
#include <stdlib.h>

#include "window.h"

static pthread_mutex_t inputMutex;

static InputState mainState;
InputState simInputState;

static void initInputState(InputState* state) {
    state->keysDown = NULL;
    state->keysHeld = NULL;
    state->keysUp = NULL;

    state->mouseButtonsDown = NULL;
    state->mouseButtonsHeld = NULL;
    state->mouseButtonsUp = NULL;

    state->mouseDeltaX = 0.0;
    state->mouseDeltaY = 0.0;
}

static void freeInputState(InputState* state) {
    hmfree(state->keysDown);
    hmfree(state->keysHeld);
    hmfree(state->keysUp);

    hmfree(state->mouseButtonsDown);
    hmfree(state->mouseButtonsHeld);
    hmfree(state->mouseButtonsUp);
}

static void cloneInputState(InputState* dest, const InputState* src) {
    freeInputState(dest);
    initInputState(dest);

    for (int i = 0; i < hmlen(src->keysDown); ++i) hmputs(dest->keysDown, src->keysDown[i]);
    for (int i = 0; i < hmlen(src->keysHeld); ++i) hmputs(dest->keysHeld, src->keysHeld[i]);
    for (int i = 0; i < hmlen(src->keysUp); ++i) hmputs(dest->keysUp, src->keysUp[i]);

    for (int i = 0; i < hmlen(src->mouseButtonsDown); ++i) hmputs(dest->mouseButtonsDown, src->mouseButtonsDown[i]);
    for (int i = 0; i < hmlen(src->mouseButtonsHeld); ++i) hmputs(dest->mouseButtonsHeld, src->mouseButtonsHeld[i]);
    for (int i = 0; i < hmlen(src->mouseButtonsUp); ++i) hmputs(dest->mouseButtonsUp, src->mouseButtonsUp[i]);

    dest->mouseDeltaX = src->mouseDeltaX;
    dest->mouseDeltaY = src->mouseDeltaY;
}

void initInput() {
    pthread_mutex_init(&inputMutex, NULL);

    initInputState(&mainState);
    initInputState(&simInputState);

    windowHideCursor();
    windowEnableInputCallbacks();
}

void freeInput() {
    pthread_mutex_destroy(&inputMutex);

    windowDisableInputCallbacks();
    windowShowCursor();

    freeInputState(&mainState);
    freeInputState(&simInputState);
}

void inputKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    switch (action) {
        case GLFW_PRESS:
            hmputs(mainState.keysDown, (MapEntry){key});
            hmputs(mainState.keysHeld, (MapEntry){key});
            break;
        case GLFW_RELEASE:
            hmputs(mainState.keysUp, (MapEntry){key});
            // if key was not pressed and released in same simulation tick, remove from held immediately
            if (hmgetp_null(mainState.keysDown, key) == NULL) {
                hmdel(mainState.keysHeld, key);
            }
            break;
        default:
            break;
    }
}
void inputMouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    switch (action) {
        case GLFW_PRESS:
            hmputs(mainState.mouseButtonsDown, (MapEntry){button});
            hmputs(mainState.mouseButtonsHeld, (MapEntry){button});
            break;
        case GLFW_RELEASE:
            hmputs(mainState.mouseButtonsUp, (MapEntry){button});
            // if button was not pressed and released in same simulation tick, remove from held immediately
            if (hmgetp_null(mainState.mouseButtonsDown, button) == NULL) {
                hmdel(mainState.mouseButtonsHeld, button);
            }
            break;
        default:
            break;
    }
}
void inputMousePosCallback(GLFWwindow* window, double xpos, double ypos) {
    mainState.mouseDeltaX += xpos;
    mainState.mouseDeltaY += ypos;
}

void inputBeforePoll() {
    pthread_mutex_lock(&inputMutex);
}
void inputAfterPoll() {
    pthread_mutex_unlock(&inputMutex);
}

void inputSimStartTick() {
    pthread_mutex_lock(&inputMutex);

    cloneInputState(&simInputState, &mainState);

    hmfree(mainState.keysDown);
    mainState.keysDown = NULL;
    for (int i = 0; i < hmlen(mainState.keysUp); ++i) {
        hmdel(mainState.keysHeld, mainState.keysUp[i].key);
    }
    hmfree(mainState.keysUp);
    mainState.keysUp = NULL;

    hmfree(mainState.mouseButtonsDown);
    mainState.mouseButtonsDown = NULL;
    for (int i = 0; i < hmlen(mainState.mouseButtonsUp); ++i) {
        hmdel(mainState.mouseButtonsHeld, mainState.mouseButtonsUp[i].key);
    }
    hmfree(mainState.mouseButtonsUp);
    mainState.mouseButtonsUp = NULL;

    mainState.mouseDeltaX = 0.0;
    mainState.mouseDeltaY = 0.0;

    pthread_mutex_unlock(&inputMutex);
}