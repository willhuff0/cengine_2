//
// Created by wehuf on 9/27/2024.
//

#ifndef DRAW_SYSTEM_H
#define DRAW_SYSTEM_H

#include "assets/shader.h"
#include "assets/simple_material.h"
#include "../renderer/renderer_object.h"
#include "assets/simple_mesh.h"

typedef enum {
    DRAW_BIND_SHADER,
    DRAW_BIND_SIMPLE_MATERIAL,
    DRAW_BIND_SIMPLE_MESH,
    DRAW_BIND_TRANSFORM,
    DRAW_SIMPLE_MESH,
} DrawInstructionType;

typedef struct {
    DrawInstructionType type;
    union DrawInstructionData {
        ShaderID shader;
        SimpleMaterialID simpleMaterial;
        RendererObjectID rendererObject;
        SimpleMeshID simpleMesh;
    } data;
} DrawInstruction;

extern DrawInstruction* drawInstructions;

void initDrawSystem();
void freeDrawSystem();

// Must be called from the main thread
void executeDrawQueue();

#endif //DRAW_SYSTEM_H
