//
// Created by wehuf on 9/27/2024.
//

#ifndef BOUNDING_BOX_H
#define BOUNDING_BOX_H

#include "../common.h"

typedef vec3 AABB[2];

typedef struct SimpleVertex SimpleVertex;

void createAABBFromSimpleVertices(AABB dest, const SimpleVertex* vertices, const int64_t numVertices);

bool aabbIsInFrustum(AABB aabb, mat4 transform, vec4 frustum[6]);

#endif //BOUNDING_BOX_H
