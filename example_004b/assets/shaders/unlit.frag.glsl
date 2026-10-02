#version 460 core

layout (std430, binding = 2) readonly buffer Colors {
  vec4 u_colors[];
};

flat in uint v_color_index;

out vec4 f_color;

void main() {
  f_color = u_colors[v_color_index];
}
