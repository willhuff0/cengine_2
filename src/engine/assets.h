//
// Created by wehuf on 9/26/2024.
//

#ifndef ASSETS_H
#define ASSETS_H

#include "assets/texture.h"

typedef struct {
    Texture* textures;
} Assets;

extern Assets assets;

void initAssets();
void freeAssets();

#endif //ASSETS_H
