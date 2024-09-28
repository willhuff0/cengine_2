//
// Created by wehuf on 9/27/2024.
//

#include "simple_mesh.h"

#include "../assets.h"

SimpleMeshID createSimpleMesh(const SimpleMaterialID material, const int64_t numVertices, const SimpleVertex* vertices, const int numIndices, const unsigned int* indices) {
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
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * (int)sizeof(unsigned int), indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(SimpleVertex), (void*)0);                                 // Positions
    glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(SimpleVertex), (void*)offsetof(SimpleVertex, normal));    // Normals

    const SimpleMesh simpleMesh { material, numIndices, vao, vbo, ebo };
    createAABBFromSimpleVertices(simpleMesh.aabb, vertices, numVertices);

    arrput(assets.simpleMeshes, simpleMesh);
    return arrlen(assets.simpleMeshes) - 1;
}

void freeSimpleMesh(const SimpleMesh* simpleMesh) {
    glDeleteVertexArrays(1, &simpleMesh->vao);
    glDeleteBuffers(1, &simpleMesh->vbo);
    glDeleteBuffers(1, &simpleMesh->ebo);
}

SimpleMesh* lookupSimpleMesh(const SimpleMeshID id) {
    return &assets.simpleMeshes[id];
}

void bindSimpleMesh(const SimpleMeshID id) {
    glBindVertexArray(lookupSimpleMesh(id)->vao);
    //bindSimpleMaterial(lookupSimpleMesh(id)->material);
}

void drawSimpleMesh(const SimpleMeshID id) {
    glDrawElements(GL_TRIANGLES, lookupSimpleMesh(id)->numIndices, GL_UNSIGNED_INT, NULL);
}

void drawSimpleMeshInstanced(const SimpleMeshID id, const int numInstances) {
    glDrawElementsInstanced(GL_TRIANGLES, lookupSimpleMesh(id)->numIndices, GL_UNSIGNED_INT, NULL, numInstances);
}