//
// Created by wehuf on 9/27/2024.
//

#ifndef STATIC_OBJECT_H
#define STATIC_OBJECT_H

#include "simple_mesh.h"

typedef enum {
    SIMPLE_OPAQUE,
    UBER_OPAQUE,
    SIMPLE_TRANSPARENT,
    UBER_TRANSPARENT,
} StaticObjectMeshType;

typedef struct {
    StaticObjectMeshType type;
    union Mesh {
        SimpleMeshID simpleMesh;
        int uberMesh;
    } mesh;
} StaticObject;

#define IS_STATIC_OBJECT_OPAQUE(staticObject) (staticObject.type == SIMPLE_OPAQUE || staticObject.type == UBER_OPAQUE)
#define IS_STATIC_OBJECT_TRANSPARENT(staticObject) (staticObject.type == SIMPLE_TRANSPARENT || staticObject.type == UBER_TRANSPARENT)

#endif //STATIC_OBJECT_H
