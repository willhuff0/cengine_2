//
// Created by wehuf on 9/26/2024.
//

#ifndef RENDERER_H
#define RENDERER_H

#include "../jobs/jobs.h"

#define RENDERER_ENABLE_CULLING

extern JobTree renderTree;

void initRenderer();
void freeRenderer();

void executeRenderTreeAsync();

#endif //RENDERER_H
