//
// Created by wehuf on 9/27/2024.
//

#ifndef RENDERER_SCENE_H
#define RENDERER_SCENE_H

#include <pthread.h>

#include "render_object.h"

extern pthread_mutex_t renderSceneMutex;

extern RenderObject* renderObjects;

void initRendererScene();
void freeRendererScene();

void registerRenderObject(SimObject* simObject);

#endif //RENDERER_SCENE_H
