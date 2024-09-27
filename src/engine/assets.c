//
// Created by wehuf on 9/26/2024.
//

#include "assets.h"

Assets assets;

void initAssets() {
    assets.textures = NULL;
}
void freeAssets() {
    arrfree(assets.textures);
}