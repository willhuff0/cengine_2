//
// Created by wehuf on 9/26/2024.
//

#ifndef SIM_OBJECT_H
#define SIM_OBJECT_H

typedef struct RenderObject RenderObject;
typedef struct VisibilityObject VisibilityObject;

typedef struct {
    RenderObject* renderObjectHandle;
    VisibilityObject* visibilityObjectHandle;
} SimObject;

#endif //SIM_OBJECT_H
