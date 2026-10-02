#version 460 core

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_uv;
layout (location = 3) in mat4 a_model;
layout (location = 7) in mat3 a_normal_matrix;
layout (location = 10) in uint a_material_index;

uniform mat4 u_view;
uniform mat4 u_projection;

out vec3 v_position;
out vec3 v_normal;

flat out uint v_material_index;

void main() {
  const vec4 world_position = a_model * vec4(a_position, 1.0F);
  v_position = world_position.xyz;
  v_normal = a_normal_matrix * a_normal;
  v_material_index = a_material_index;
  gl_Position = u_projection * u_view * world_position;
}
