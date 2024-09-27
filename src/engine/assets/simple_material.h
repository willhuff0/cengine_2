//
// Created by wehuf on 9/27/2024.
//

#ifndef SIMPLE_MATERIAL_H
#define SIMPLE_MATERIAL_H

#include "shader.h"

typedef int SimpleMaterialID;

typedef struct {
    ShaderID shader;
    vec4 color;
} SimpleMaterial;

// Adds a simple material to assets and returns its ID
// Returns -1 if an error occurs
SimpleMaterialID createSimpleMaterial(const ShaderID shader, vec4 color);

void freeSimpleMaterial(const SimpleMaterial* simpleMaterial);

SimpleMaterial* lookupSimpleMaterial(const SimpleMaterialID id);

void bindSimpleMaterial(const SimpleMaterialID id);

#endif //SIMPLE_MATERIAL_H
