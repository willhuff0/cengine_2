//
// Created by wehuf on 9/27/2024.
//

#include "simple_mesh.h"

SimpleMeshID createSimpleMesh(const SimpleMaterialID material, const int64_t numVertices, const SimpleVertex* vertices, const int64_t numIndices, const unsigned int* indices) {
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, numVertices * (int64_t)sizeof(SimpleVertex), vertices, GL_STATIC_DRAW);

    GLuint ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * (int64_t)sizeof(unsigned int), indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(SimpleVertex), (void*)0);                                 // Positions
    glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(SimpleVertex), (void*)offsetof(SimpleVertex, normal));    // Normals

    SimpleMesh* mesh = malloc(sizeof(SimpleMesh));
    mesh->material = material;
    mesh->vao = vao;
    mesh->vbo = vbo;
    mesh->ebo = ebo;
    mesh->numIndices = numIndices;
    arrput(scene.simpleMeshes, mesh);
    if (outMesh != NULL) *outMesh = mesh;
}

void freeSimpleMesh(const SimpleMesh* simpleMesh);

SimpleMesh* lookupSimpleMesh(const SimpleMeshID id);

void bindSimpleMesh(const SimpleMeshID id);
void drawSimpleMesh(const SimpleMeshID id);
void drawSimpleMeshInstanced(const SimpleMeshID id, int numInstances);