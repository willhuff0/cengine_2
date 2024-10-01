//
// Created by wehuf on 9/27/2024.
//

#ifndef RENDERER_OBJECT_H
#define RENDERER_OBJECT_H

#include "../engine/assets.h"

typedef int RendererObjectID;

typedef enum {
    SIMPLE_OPAQUE_RENDERER_OBJECT,
    UBER_OPAQUE_RENDERER_OBJECT,
    SIMPLE_TRANSPARENT_RENDERER_OBJECT,
    UBER_TRANSPARENT_RENDERER_OBJECT,
} RendererObjectMeshType;

typedef struct {
    mat4 matrix;
} RendererObjectTransform;

typedef struct {
    RendererObjectMeshType type;
    union Mesh {
        SimpleMeshID simpleMesh;
        int uberMesh;
    } mesh;

    RendererObjectTransform transform;
} RendererObject;

#define IS_RENDERER_OBJECT_OPAQUE(rendererObjectPtr) (rendererObjectPtr->type == SIMPLE_OPAQUE_RENDERER_OBJECT || rendererObjectPtr->type == UBER_OPAQUE_RENDERER_OBJECT)
#define IS_RENDERER_OBJECT_TRANSPARENT(rendererObjectPtr) (rendererObjectPtr->type == SIMPLE_TRANSPARENT_RENDERER_OBJECT || rendererObjectPtr->type == UBER_TRANSPARENT_RENDERER_OBJECT)

#define IS_RENDERER_OBJECT_SIMPLE(rendererObjectPtr) (rendererObjectPtr->type == SIMPLE_OPAQUE_RENDERER_OBJECT || rendererObjectPtr->type == SIMPLE_TRANSPARENT_RENDERER_OBJECT)
#define IS_RENDERER_OBJECT_UBER(rendererObjectPtr) (rendererObjectPtr->type == UBER_OPAQUE_RENDERER_OBJECT || rendererObjectPtr->type == UBER_TRANSPARENT_RENDERER_OBJECT)

//#define GET_RENDERER_OBJECT_AABB(rendererObjectPtr) (IS_RENDERER_OBJECT_SIMPLE(rendererObjectPtr) ? lookupSimpleMesh(rendererObjectPtr->mesh.simpleMesh)->aabb : lookupUberMesh(rendererObjectPtr->mesh.uberMesh)->aabb)
#define GET_RENDERER_OBJECT_AABB(rendererObjectPtr) (lookupSimpleMesh(rendererObjectPtr->mesh.simpleMesh)->aabb)

extern RendererObject* rendererObjects;

void initRendererObjects();
void freeRendererObjects();

// Should only be called from a renderer tree job
// Returns -1 if an error occurs
RendererObjectID registerSimpleRendererObject(const SimpleMeshID simpleMesh, const bool isTransparent);

RendererObject* lookupRendererObject(const RendererObjectID id);

void bindRendererObjectTransform(const RendererObjectID id);

#endif //RENDERER_OBJECT_H
