//
// Created by wehuf on 9/27/2024.
//

#include "renderer_scene.h"

pthread_mutex_t renderSceneMutex;

RenderObject* renderObjects;

void initRendererScene() {
    pthread_mutex_init(&renderSceneMutex, NULL);
    renderObjects = NULL;
}
void freeRendererScene() {
    pthread_mutex_destroy(&renderSceneMutex);
    arrfree(renderObjects);
}

void registerRenderObject(SimObject* simObject);