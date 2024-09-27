//
// Created by wehuf on 9/27/2024.
//

#include "simple_material.h"

SimpleMaterialID createSimpleMaterial(ShaderID shader, vec4 color);

void freeSimpleMaterial(const SimpleMaterial* simpleMaterial);

SimpleMaterial* lookupSimpleMaterial(const SimpleMaterialID id);

void bindSimpleMaterial(const SimpleMaterialID id);