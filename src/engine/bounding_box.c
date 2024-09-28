//
// Created by wehuf on 9/27/2024.
//

#include "bounding_box.h"

#include <minwindef.h>

void createAABBFromSimpleVertices(AABB dest, const SimpleVertex* vertices, const int numVertices) {
    assert(numVertices > 0);

    float minX = FLT_MAX;
    float maxX = -FLT_MAX;

    float minY = FLT_MAX;
    float maxY = -FLT_MAX;

    float minZ = FLT_MAX;
    float maxZ = -FLT_MAX;

    for (int i = 0; i < numVertices; ++i) {
        const SimpleVertex* vertex = &vertices[i];

        if (vertex->position[0] < minX) minX = vertex->position[0];
        if (vertex->position[0] > maxX) maxX = vertex->position[0];

        if (vertex->position[1] < minY) minY = vertex->position[1];
        if (vertex->position[1] > maxY) maxY = vertex->position[1];

        if (vertex->position[2] < minZ) minZ = vertex->position[2];
        if (vertex->position[2] > maxZ) maxZ = vertex->position[2];
    }

    dest[0][0] = minX;
    dest[0][1] = minY;
    dest[0][2] = minZ;

    dest[1][0] = maxX;
    dest[1][1] = maxY;
    dest[1][2] = maxZ;
}

bool aabbIsInFrustum(AABB aabb, mat4 transform, vec4 frustum[6]) {
    vec3 transformedAABB[2];
    glm_aabb_transform(aabb, transform, transformedAABB);

    return glm_aabb_frustum(transformedAABB, frustum);
}