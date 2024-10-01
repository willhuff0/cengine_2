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
    const RendererObject rendererObject = {
        .type = isTransparent ? SIMPLE_TRANSPARENT_RENDERER_OBJECT : SIMPLE_OPAQUE_RENDERER_OBJECT,
        .mesh = { .simpleMesh = simpleMesh },
    };
    glm_mat4_identity(rendererObject.transform.matrix);

    arrput(rendererObjects, rendererObject);
    return arrlen(rendererObjects) - 1;
}

RendererObject* lookupRendererObject(const RendererObjectID id) {
    return &rendererObjects[id];
}

void bindRendererObjectTransform(const RendererObjectID id) {
    setUniformMat4At(0, lookupRendererObject(id)->transform.matrix);
}
