//
// Created by wehuf on 9/26/2024.
//

#include "input.h"

#include <pthread.h>
#include <stdlib.h>

#include "window.h"

static typedef struct {
    int key;
} MapEntry;

MapEntry* keysDown;
MapEntry* keysHeld;
MapEntry* keysUp;

MapEntry* mouseButtonsDown;
MapEntry* mouseButtonsHeld;
MapEntry* mouseButtonsUp;

double mouseDeltaX;
double mouseDeltaY;

void initInput() {
    keysDown = NULL;
    keysHeld = NULL;

    mouseButtonsDown = NULL;
    mouseButtonsHeld = NULL;
    mouseButtonsUp = NULL;

    mouseDeltaX = 0.0;
    mouseDeltaY = 0.0;

    windowHideCursor();
    windowEnableInputCallbacks();
}

void freeInput() {
    windowDisableInputCallbacks();
    windowShowCursor();

    hmfree(keysDown);
    hmfree(keysHeld);
    hmfree(keysUp);

    hmfree(mouseButtonsDown);
    hmfree(mouseButtonsHeld);
    hmfree(mouseButtonsUp);
}

void inputKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    switch (action) {
        case GLFW_PRESS:
            hmputs(keysDown, (MapEntry){key});
            hmputs(keysHeld, (MapEntry){key});
            break;
        case GLFW_RELEASE:
            hmputs(keysUp, (MapEntry){key});
            break;
        default:
            break;
    }
}
void inputMouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    switch (action) {
        case GLFW_PRESS:
            hmputs(mouseButtonsDown, (MapEntry){button});
            hmputs(mouseButtonsHeld, (MapEntry){button});
            break;
        case GLFW_RELEASE:
            hmputs(mouseButtonsUp, (MapEntry){button});
            break;
        default:
            break;
    }
}
void inputMousePosCallback(GLFWwindow* window, double xpos, double ypos) {
    mouseDeltaX += xpos;
    mouseDeltaY += ypos;
}

void inputBeforePoll() {
    hmfree(keysDown);
    keysDown = NULL;
    for (int i = 0; i < hmlen(keysUp); ++i) {
        hmdel(keysHeld, keysUp[i].key);
    }
    hmfree(keysUp);
    keysUp = NULL;

    hmfree(mouseButtonsDown);
    mouseButtonsDown = NULL;
    for (int i = 0; i < hmlen(mouseButtonsUp); ++i) {
        hmdel(mouseButtonsHeld, mouseButtonsUp[i].key);
    }
    hmfree(mouseButtonsUp);
    mouseButtonsUp = NULL;

    mouseDeltaX = 0.0;
    mouseDeltaY = 0.0;
}

// Thread safe read functions

bool isKeyDown(int key) { return hmgetp_null(keysDown, key) != NULL; }
bool isKeyHeld(int key) { return hmgetp_null(keysHeld, key) != NULL; }
bool isKeyUp(int key) { return hmgetp_null(keysUp, key) != NULL; }

bool isMouseButtonDown(int button) { return hmgetp_null(mouseButtonsDown, button) != NULL; }
bool isMouseButtonHeld(int button) { return hmgetp_null(mouseButtonsHeld, button) != NULL; }
bool isMouseButtonUp(int button) { return hmgetp_null(mouseButtonsUp, button) != NULL; }

double getMouseDeltaX() { return mouseDeltaX; }
double getMouseDeltaY() { return mouseDeltaY; }

//