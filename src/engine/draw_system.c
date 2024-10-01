//
// Created by wehuf on 9/27/2024.
//

#include "draw_system.h"

#include "window.h"
#include "../renderer/frame_packet.h"

DrawInstruction* drawInstructions;

static GLuint cengineUbo;

void initDrawSystem() {
    drawInstructions = NULL;

    glGenBuffers(1, &cengineUbo);
    glBindBuffer(GL_UNIFORM_BUFFER, cengineUbo);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(mat4), NULL, GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, cengineUbo);
}

void freeDrawSystem() {
    if (drawInstructions != NULL) {
        arrfree(drawInstructions);
        drawInstructions = NULL;
    }

    glDeleteBuffers(1, &cengineUbo);
}

void extractPlanesFromViewProjMat(const mat4 viewProjMat, vec4 planes[6]) {
    for (int i = 4; i--;) { planes[0][i] = viewProjMat[i][3] + viewProjMat[i][0]; }
    for (int i = 4; i--;) { planes[1][i] = viewProjMat[i][3] - viewProjMat[i][0]; }
    for (int i = 4; i--;) { planes[2][i] = viewProjMat[i][3] + viewProjMat[i][1]; }
    for (int i = 4; i--;) { planes[3][i] = viewProjMat[i][3] - viewProjMat[i][1]; }
    for (int i = 4; i--;) { planes[4][i] = viewProjMat[i][3] + viewProjMat[i][2]; }
    for (int i = 4; i--;) { planes[5][i] = viewProjMat[i][3] - viewProjMat[i][2]; }
}

static void makeViewProjMat(vec3 cameraPosition, vec3 cameraForward, mat4 dest) {
    mat4 proj;
    glm_perspective(glm_rad(70.0f), (float)getWindowWidth() / (float)getWindowHeight(), 0.1f, 300.0f, proj);

    vec3 lookTarget;
    glm_vec3_add(cameraPosition, cameraForward, lookTarget);
    mat4 view;
    glm_lookat(cameraPosition, lookTarget, (vec3){0.0f, 1.0f, 0.0f}, view);
    glm_lookat(cameraPosition, (vec3) {0.0f, 0.0f, 0.0f}, (vec3){0.0f, 1.0f, 0.0f}, view);

    glm_mat4_mul(proj, view, dest);
}

void executeDrawQueue() {
    // Prep OpenGL for drawing
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //makeViewProjMat(rendererFramePacket->view.pos, rendererFramePacket->view.rot, rendererFramePacket->view.viewProjMat);
    mat4 viewProjMat;
    makeViewProjMat(rendererFramePacket->view.pos, (vec3) {0.0f, 0.0f, 1.0f}, viewProjMat);
    glBindBuffer(GL_UNIFORM_BUFFER, cengineUbo);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(mat4), viewProjMat);

    extractPlanesFromViewProjMat(viewProjMat, rendererFramePacket->view.frustum);

    int numDrawCalls = 0;

    // Execute draw instructions
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
            case DRAW_BIND_TRANSFORM:
                bindRendererObjectTransform(drawInstructions[i].data.rendererObject);
                break;
            case DRAW_SIMPLE_MESH:
                numDrawCalls++;
                drawSimpleMesh(drawInstructions[i].data.simpleMesh);
                break;
        }
    }

    printf("Number of draw calls: %d\n", numDrawCalls);

    arrfree(drawInstructions);
    drawInstructions = NULL;
}