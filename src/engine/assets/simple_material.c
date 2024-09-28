//
// Created by wehuf on 9/27/2024.
//

#include "simple_material.h"

#include "../assets.h"

SimpleMaterialID createSimpleMaterial(const ShaderID shader, vec4 color) {
    SimpleMaterial simpleMaterial { shader };
    glm_vec4_copy(color, simpleMaterial.color);

    arrput(assets.simpleMaterials, simpleMaterial);
    return arrlen(assets.simpleMaterials) - 1;
}

void freeSimpleMaterial(const SimpleMaterial* simpleMaterial) { }

SimpleMaterial* lookupSimpleMaterial(const SimpleMaterialID id) {
    return &assets.simpleMaterials[id];
}

void bindSimpleMaterial(const SimpleMaterialID id) {
    //bindShader(lookupSimpleMaterial(id)->shader);
}