#version 460 core

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_uv;
layout (location = 3) in mat4 a_model;
layout (location = 10) in uint a_material_index;

uniform mat4 u_view;
uniform mat4 u_projection;

flat out uint v_material_index;

void main() {
  v_material_index = a_material_index;
  gl_Position = u_projection * u_view * a_model * vec4(a_position, 1.0F);
}
