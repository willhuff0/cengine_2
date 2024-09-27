//
// Created by wehuf on 9/26/2024.
//

#ifndef TEXTURE_H
#define TEXTURE_H

#include "../../common.h"

typedef int TextureID;

typedef struct {
    GLuint texture;
} Texture;

// Adds a texture to assets and returns its ID
// Returns -1 if an error occurs
TextureID createTextureFromPath(const char* path, const bool generateMipmaps);

// Adds a texture to assets and returns its ID
// Returns -1 if an error occurs
TextureID createTextureFromData(const char* filename, const uint8_t* buffer, const int length, const bool generateMipmaps);

Texture* lookupTexture(const TextureID id);

void freeTexture(const Texture* texture);

void bindTexture(const TextureID id, const GLenum textureUnit);

#endif //TEXTURE_H
