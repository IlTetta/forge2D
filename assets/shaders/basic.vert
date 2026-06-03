#version 330 core

layout(location = 0) in vec2  a_position;
layout(location = 1) in vec2  a_texcoord;
layout(location = 2) in vec4  a_color;
layout(location = 3) in float a_texIndex;

uniform mat4 u_viewProjection;

out vec2      v_texcoord;
out vec4      v_color;
flat out int  v_texIndex;   // flat = no interpolation, stays as per-vertex integer

void main() {
    v_texcoord  = a_texcoord;
    v_color     = a_color;
    v_texIndex  = int(a_texIndex);
    gl_Position = u_viewProjection * vec4(a_position, 0.0, 1.0);
}
