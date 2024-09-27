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

#endif //SIMPLE_MATERIAL_H
