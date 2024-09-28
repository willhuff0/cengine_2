//
// Created by wehuf on 9/27/2024.
//

#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "../common.h"

typedef struct {
  vec3 position;
  vec3 eulerAngles;
  vec3 scale;
} Transform;

void transformGetModelMatrix(const Transform* transform, mat4 dest);

#endif //TRANSFORM_H
