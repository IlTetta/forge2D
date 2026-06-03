#version 330 core

in vec2      v_texcoord;
in vec4      v_color;
flat in int  v_texIndex;

// One sampler per texture slot — each bound to the matching GL texture unit
uniform sampler2D u_textures[16];

out vec4 fragColor;

void main() {
    // Dynamic sampler indexing requires a modern driver (OpenGL 3.3+, all desktop GPUs).
    // If this fails, replace with a switch(v_texIndex){case 0:... case 1:...} block.
    fragColor = texture(u_textures[v_texIndex], v_texcoord) * v_color;
}
