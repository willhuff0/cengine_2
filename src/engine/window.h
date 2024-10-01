//
// Created by wehuf on 9/26/2024.
//

#ifndef WINDOW_H
#define WINDOW_H

#include "../common.h"

void initWindow();
void freeWindow();

bool windowShouldClose();
void windowPollEvents();
void windowSwapBuffers();

void windowHideCursor();
void windowShowCursor();

void windowEnableInputCallbacks();
void windowDisableInputCallbacks();

const char* windowGetGLFWVersionString();

int getWindowWidth();
int getWindowHeight();

#endif //WINDOW_H
