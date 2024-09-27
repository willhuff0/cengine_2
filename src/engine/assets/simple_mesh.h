//
// Created by wehuf on 9/27/2024.
//

#ifndef SIMPLE_MESH_H
#define SIMPLE_MESH_H

#include "simple_material.h"

typedef int SimpleMeshID;

typedef struct {
    vec3 position;
    vec3 normal;
} SimpleVertex;

typedef struct {
    SimpleMaterialID material;

    int64_t numIndices;
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
} SimpleMesh;

// Adds a simple mesh to assets and returns its ID
// Returns -1 if an error occurs
SimpleMeshID createSimpleMesh(const SimpleMaterialID material, const int64_t numVertices, const SimpleVertex* vertices, const int64_t numIndices, const unsigned int* indices);

void freeSimpleMesh(const SimpleMesh* simpleMesh);

SimpleMesh* lookupSimpleMesh(const SimpleMeshID id);

void bindSimpleMesh(const SimpleMeshID id);
void drawSimpleMesh(const SimpleMeshID id);
void drawSimpleMeshInstanced(const SimpleMeshID id, int numInstances);

#endif //SIMPLE_MESH_H
