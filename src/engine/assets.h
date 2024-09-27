//
// Created by wehuf on 9/26/2024.
//

#ifndef ASSETS_H
#define ASSETS_H

#include "assets/shader.h"
#include "assets/simple_material.h"
#include "assets/simple_mesh.h"
#include "assets/texture.h"

typedef struct {
    Texture* textures;
    Shader* shaders;
    SimpleMaterial* simpleMaterials;
    SimpleMesh* simpleMeshes;
} Assets;

extern Assets assets;

void initAssets();
void freeAssets();

#endif //ASSETS_H
