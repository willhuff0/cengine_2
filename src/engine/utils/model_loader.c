//
// Created by wehuf on 10/1/2024.
//

#include "model_loader.h"

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

static SimpleMeshID addSimpleMesh(const SimpleMaterialID simpleMaterial, const struct aiMesh* mesh) {
    SimpleVertex* vertices = malloc(mesh->mNumVertices * sizeof(SimpleVertex));
    unsigned int* indices = malloc(mesh->mNumFaces * 3 * sizeof(unsigned int));

    for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
        SimpleVertex vertex;

        vertex.position[0] = mesh->mVertices[i].x;
        vertex.position[1] = mesh->mVertices[i].y;
        vertex.position[2] = mesh->mVertices[i].z;

        vertex.normal[0] = mesh->mNormals[i].x;
        vertex.normal[1] = mesh->mNormals[i].y;
        vertex.normal[2] = mesh->mNormals[i].z;

        vertices[i] = vertex;
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
        const struct aiFace face = mesh->mFaces[i];
        indices[i * 3 + 0] = face.mIndices[0];
        indices[i * 3 + 1] = face.mIndices[1];
        indices[i * 3 + 2] = face.mIndices[2];
    }

    const SimpleMeshID simpleMesh = createSimpleMesh(simpleMaterial, mesh->mNumVertices, vertices, (int)mesh->mNumFaces * 3, indices);

    free(vertices);
    free(indices);

    return simpleMesh;
}

bool loadSimpleModel(const char* path, const SimpleMaterialID simpleMaterial, SimpleModel* dest) {
    const struct aiScene* scene = aiImportFile(path,
                                        aiProcess_Triangulate |
                                        aiProcess_PreTransformVertices |
                                        aiProcess_JoinIdenticalVertices |
                                        aiProcess_FlipUVs |
                                        aiProcess_GenNormals);

    if (scene == NULL) {
        fprintf(stderr, "[MODEL LOADER] Failed to load assimp scene: %s\n", aiGetErrorString());
        return false;
    }
    if (scene->mNumMeshes < 1) {
        fprintf(stderr, "[MODEL LOADER] assimp scene has no meshes: %s\n", path);
        return false;
    }

    dest->numMeshes = scene->mNumMeshes;
    dest->meshes = malloc(scene->mNumMeshes * sizeof(SimpleMeshID));

    for (int i = 0; i < scene->mNumMeshes; ++i) {
        dest->meshes[i] = addSimpleMesh(simpleMaterial, scene->mMeshes[i]);
    }

    aiReleaseImport(scene);
    return true;
}