//
// Created by wehuf on 9/26/2024.
//

#ifndef INPUT_OLD_H
#define INPUT_OLD_H

#include <pthread.h>

#include "../common.h"

typedef struct {
    int key;
} MapEntry;

typedef struct {
    MapEntry* keysDown;
    MapEntry* keysHeld;
    MapEntry* keysUp;

    MapEntry* mouseButtonsDown;
    MapEntry* mouseButtonsHeld;
    MapEntry* mouseButtonsUp;

    double mouseDeltaX;
    double mouseDeltaY;
} InputState;

extern InputState simInputState;

void initInput();
void freeInput();

void inputKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void inputMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void inputMousePosCallback(GLFWwindow* window, double xpos, double ypos);

void inputBeforePoll();
void inputAfterPoll();

void inputSimStartTick();

#endif //INPUT_OLD_H
