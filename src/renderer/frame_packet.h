//
// Created by wehuf on 9/26/2024.
//

#ifndef FRAME_PACKET_H
#define FRAME_PACKET_H

#include "../common.h"

typedef struct {
    vec3 pos;
    vec3 rot;
    mat4 viewProjMat;
} FramePacketView;

typedef struct {
    float time;
    float deltaTime;

    FramePacketView view;
} FramePacket;

extern FramePacket* rendererFramePacket;
extern FramePacket* simFramePacket;

void initFramePackets();
void freeFramePackets();

void swapFramePackets();

#endif //FRAME_PACKET_H
