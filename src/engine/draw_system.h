//
// Created by wehuf on 9/27/2024.
//

#ifndef DRAW_SYSTEM_H
#define DRAW_SYSTEM_H

#include "assets/shader.h"
#include "assets/simple_material.h"
#include "assets/simple_mesh.h"

typedef enum {
    DRAW_BIND_SHADER,
    DRAW_BIND_SIMPLE_MATERIAL,
    DRAW_BIND_SIMPLE_MESH,
    DRAW_SIMPLE_MESH,
} DrawInstructionType;

typedef struct {
    DrawInstructionType type;
    union DrawInstructionData {
        ShaderID shader;
        SimpleMaterialID simpleMaterial;
        SimpleMeshID simpleMesh;
    } data;
} DrawInstruction;

extern DrawInstruction* drawInstructions;

#endif //DRAW_SYSTEM_H
