//
// Created by wehuf on 10/1/2024.
//

#ifndef MODEL_LOADER_H
#define MODEL_LOADER_H

#include "../assets/simple_mesh.h"

typedef struct {
    unsigned int numMeshes;
    SimpleMeshID* meshes;
} SimpleModel;

bool loadSimpleModel(const char* path, const SimpleMaterialID simpleMaterial, SimpleModel* dest);

#endif //MODEL_LOADER_H
