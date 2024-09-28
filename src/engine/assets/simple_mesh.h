//
// Created by wehuf on 9/27/2024.
//

#ifndef SIMPLE_MESH_H
#define SIMPLE_MESH_H

#include "simple_material.h"
#include "../bounding_box.h"

typedef int SimpleMeshID;

struct SimpleVertex {
    vec3 position;
    vec3 normal;
};

typedef struct {
    SimpleMaterialID material;

    int numIndices;
    GLuint vao;
    GLuint vbo;
    GLuint ebo;

    AABB aabb;
} SimpleMesh;

// Adds a simple mesh to assets and returns its ID
// Returns -1 if an error occurs
SimpleMeshID createSimpleMesh(const SimpleMaterialID material, const int64_t numVertices, const SimpleVertex* vertices, const int numIndices, const unsigned int* indices);

void freeSimpleMesh(const SimpleMesh* simpleMesh);

SimpleMesh* lookupSimpleMesh(const SimpleMeshID id);

void bindSimpleMesh(const SimpleMeshID id);
void drawSimpleMesh(const SimpleMeshID id);
void drawSimpleMeshInstanced(const SimpleMeshID id, const int numInstances);

#endif //SIMPLE_MESH_H
