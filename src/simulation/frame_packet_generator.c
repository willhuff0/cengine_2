//
// Created by wehuf on 9/26/2024.
//

#include "frame_packet_generator.h"

#include "../renderer/frame_packet.h"

void generateFramePacket() {
    glm_vec3_copy((vec3){-10.0f + (float)glfwGetTime(), 0.0f, 0.0f}, simFramePacket->pos);
}
