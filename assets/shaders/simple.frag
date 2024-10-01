#version 320 es
precision highp float;

layout(location = 1) uniform vec4 u_color;

out vec4 o_fragColor;

void main() {
    o_fragColor = u_color;
}