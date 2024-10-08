//
// Created by wehuf on 9/26/2024.
//

#include "assets.h"

Assets assets;

void initAssets() {
    assets.textures = NULL;
    assets.shaders = NULL;
    assets.simpleMaterials = NULL;
    assets.simpleMeshes = NULL;
}

void freeAssets() {
    for (int i = 0; i < arrlen(assets.textures); ++i) {
        freeTexture(&assets.textures[i]);
    }
    arrfree(assets.textures);

    for (int i = 0; i < arrlen(assets.shaders); ++i) {
        freeShader(&assets.shaders[i]);
    }
    arrfree(assets.shaders);

    for (int i = 0; i < arrlen(assets.simpleMaterials); ++i) {
        freeSimpleMaterial(&assets.simpleMaterials[i]);
    }
    arrfree(assets.simpleMaterials);

    for (int i = 0; i < arrlen(assets.simpleMeshes); ++i) {
        freeSimpleMesh(&assets.simpleMeshes[i]);
    }
    arrfree(assets.simpleMeshes);
}