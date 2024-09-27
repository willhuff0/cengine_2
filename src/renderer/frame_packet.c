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

void swapFramePackets() {
    FramePacket* temp = rendererFramePacket;
    rendererFramePacket = simFramePacket;
    simFramePacket = temp;
}