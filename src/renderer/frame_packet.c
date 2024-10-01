//
// Created by wehuf on 9/26/2024.
//

#include "frame_packet.h"

FramePacket* rendererFramePacket;
FramePacket* simFramePacket;

void initFramePackets() {
    rendererFramePacket = (FramePacket*)malloc(sizeof(FramePacket));
    simFramePacket = (FramePacket*)malloc(sizeof(FramePacket));

    resetFramePacket(rendererFramePacket);
    resetFramePacket(simFramePacket);
}
void freeFramePackets() {
    free(rendererFramePacket);
    free(simFramePacket);
}

void resetFramePacket(FramePacket* framePacket) {
    framePacket->time = 0.0f;
    framePacket->deltaTime = 0.0f;

    glm_vec3_zero(framePacket->view.pos);
    glm_vec3_zero(framePacket->view.rot);
    glm_mat4_identity(framePacket->view.viewProjMat);

    // TODO: Set everything to zero, clear change buffers
}

void swapFramePackets() {
    FramePacket* temp = rendererFramePacket;
    rendererFramePacket = simFramePacket;
    simFramePacket = temp;
}