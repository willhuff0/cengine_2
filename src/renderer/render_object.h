//
// Created by wehuf on 9/26/2024.
//

#ifndef RENDER_OBJECT_H
#define RENDER_OBJECT_H

#include "../engine/assets.h"
#include "../engine/bounding_box.h"

typedef struct SimObject SimObject;

typedef enum {
    SIMPLE_OPAQUE,
    UBER_OPAQUE,
    SIMPLE_TRANSPARENT,
    UBER_TRANSPARENT,
} RenderObjectMeshType;

typedef struct {
    SimObject* simObjectHandle;

    RenderObjectMeshType type;
    union RenderObjectMesh {
        SimpleMeshID simpleMesh;
        int uberMesh;
    } mesh;
} RenderObject;

#define IS_OPAQUE(renderObject) (renderObject.type == SIMPLE_OPAQUE || renderObject.type == UBER_OPAQUE)
#define IS_TRANSPARENT(renderObject) (renderObject.type == SIMPLE_TRANSPARENT || renderObject.type == UBER_TRANSPARENT)

#endif //RENDER_OBJECT_H
