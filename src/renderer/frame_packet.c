//
// Created by wehuf on 9/26/2024.
//

#include "frame_packet.h"

FramePacket* rendererFramePacket;
FramePacket* simFramePacket;

void initFramePackets() {
    rendererFramePacket = (FramePacket*)malloc(sizeof(FramePacket));
    simFramePacket = (FramePacket*)malloc(sizeof(FramePacket));
}
void freeFramePackets() {
    free(rendererFramePacket);
    free(simFramePacket);
}

void resetFramePacket(FramePacket* framePacket) {
    framePacket->time = 0.0f;
    framePacket->deltaTime = 0.0f;

    // TODO: Set everything to zero, clear change buffers
}

void swapFramePackets() {
    FramePacket* temp = rendererFramePacket;
    rendererFramePacket = simFramePacket;
    simFramePacket = temp;
}