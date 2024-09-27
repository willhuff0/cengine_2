//
// Created by wehuf on 9/26/2024.
//

#ifndef INPUT_H
#define INPUT_H

#include <pthread.h>

#include "../common.h"

void initInput();
void freeInput();

void inputKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void inputMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void inputMousePosCallback(GLFWwindow* window, double xpos, double ypos);

void inputBeforePoll();

// Thread safety: can be called from any thread (only reads)

bool isKeyDown(int key);
bool isKeyHeld(int key);
bool isKeyUp(int key);

bool isMouseButtonDown(int button);
bool isMouseButtonHeld(int button);
bool isMouseButtonUp(int button);

double getMouseDeltaX();
double getMouseDeltaY();

//

#endif //INPUT_H
