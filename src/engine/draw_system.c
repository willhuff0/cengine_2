//
// Created by wehuf on 9/27/2024.
//

#include "draw_system.h"

DrawInstruction* drawInstructions;

void initDrawSystem() {
    drawInstructions = NULL;
}

void freeDrawSystem() {
    if (drawInstructions != NULL) {
        arrfree(drawInstructions);
        drawInstructions = NULL;
    }
}

void executeDrawQueue() {
    for (int i = 0; i < arrlen(drawInstructions); ++i) {
        switch (drawInstructions[i].type) {
            case DRAW_BIND_SHADER:
                bindShader(drawInstructions[i].data.shader);
                break;
            case DRAW_BIND_SIMPLE_MATERIAL:
                bindSimpleMaterial(drawInstructions[i].data.simpleMaterial);
                break;
            case DRAW_BIND_SIMPLE_MESH:
                bindSimpleMesh(drawInstructions[i].data.simpleMesh);
                break;
            case DRAW_SIMPLE_MESH:
                drawSimpleMesh(drawInstructions[i].data.simpleMesh);
                break;
        }
    }

    arrfree(drawInstructions);
    drawInstructions = NULL;
}