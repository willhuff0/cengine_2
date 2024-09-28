//
// Created by wehuf on 9/27/2024.
//

#include "renderer_object.h"

RendererObject* rendererObjects;

void initRendererObjects() {
    rendererObjects = NULL;
}

void freeRendererObjects() {
    arrfree(rendererObjects);
}

RendererObjectID registerSimpleRendererObject(const SimpleMeshID simpleMesh, const bool isTransparent) {
    const RendererObject rendererObject {
        .type = isTransparent ? SIMPLE_TRANSPARENT_RENDERER_OBJECT : SIMPLE_OPAQUE_RENDERER_OBJECT,
        .mesh = { .simpleMesh = simpleMesh },
        .transform = { .pos = { 0.0f, 0.0f, 0.0f }, .rot = { 0.0f, 0.0f, 0.0f, 1.0f }, .scale = { 1.0f, 1.0f, 1.0f }, },
    };
    glm_quat_identity(rendererObject.transform.rot);
    glm_mat4_identity(rendererObject.transform.matrix);

    arrput(rendererObjects, rendererObject);
    return arrlen(rendererObjects) - 1;
}

RendererObject* lookupRendererObject(const RendererObjectID id) {
    return &rendererObjects[id];
}

