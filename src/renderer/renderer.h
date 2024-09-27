//
// Created by wehuf on 9/26/2024.
//

#ifndef RENDERER_H
#define RENDERER_H

#include "../jobs/jobs.h"

extern Job renderTreeExit;

void initRenderer();
void freeRenderer();

void executeRenderTreeAsync();

#endif //RENDERER_H
