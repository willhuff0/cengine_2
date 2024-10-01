//
// Created by wehuf on 9/26/2024.
//

#include "frame_packet_generator.h"

#include "../renderer/frame_packet.h"

void generateFramePacket() {
    resetFramePacket(simFramePacket);

    const float t = (float)glfwGetTime();
    const float r = 6.0f;

    glm_vec3_copy((vec3){cosf(t) * r, 3.0f, sinf(t) * r}, simFramePacket->view.pos);
}
