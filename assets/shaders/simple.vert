#version 320 es

layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec3 a_normal;

// cengine_2 model matrix is always at location 0
layout(location = 0) uniform mat4 u_modelMat;

layout(std140) uniform CEngine {
    mat4 viewProjMat;
} cengine;

void main() {
    gl_Position = cengine.viewProjMat * u_modelMat * vec4(a_pos, 1.0f);
}