//
// Created by wehuf on 9/26/2024.
//

#include "texture.h"

#include "../assets.h"

static TextureID createTexture(uint8_t* data, const int width, const int height, const GLint wrap, const bool generateMipmaps) {
    GLuint gl_texture;
    glGenTextures(1, &gl_texture);
    glBindTexture(GL_TEXTURE_2D, gl_texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, generateMipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    if (generateMipmaps) glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    Texture texture {gl_texture};
    arrput(assets.textures, texture);
    return arrlen(assets.textures) - 1;
}

TextureID createTextureFromPath(const char* path, const bool generateMipmaps) {
    int width, height, numChannels;
    uint8_t* data = stbi_load(path, &width, &height, &numChannels, 3);
    if (data == NULL) {
        fprintf(stderr, "[TEXTURE] Failed to load image from path: %s\n", path);
        stbi_image_free(data);
        return -1;
    }

    return createTexture(data, width, height, GL_REPEAT, generateMipmaps);
}

TextureID createTextureFromData(const char* filename, const uint8_t* buffer, const int length, const bool generateMipmaps) {
    int width, height, numChannels;
    uint8_t* data = stbi_load_from_memory(buffer, length, &width, &height, &numChannels, 3);
    if (data == NULL) {
        fprintf(stderr, "[TEXTURE] Failed to load image from data: %s\n", filename);
        stbi_image_free(data);
        return false;
    }

    return createTexture(data, width, height, GL_REPEAT, generateMipmaps);
}

Texture* lookupTexture(const TextureID id) {
    return &assets.textures[id];
}

void freeTexture(const Texture* texture) {
    glDeleteTextures(1, &texture->texture);
}

void bindTexture(const TextureID id, const GLenum textureUnit) {
    glActiveTexture(textureUnit);
    glBindTexture(GL_TEXTURE_2D, lookupTexture(id)->texture);
}