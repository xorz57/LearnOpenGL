#version 460 core

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_uv;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

out vec3 v_position;
out vec3 v_normal;

void main() {
  const vec4 world_position = u_model * vec4(a_position, 1.0F);
  v_position = world_position.xyz;
  const mat3 model = mat3(u_model);
  const mat3 normal_matrix = mat3(cross(model[1], model[2]), cross(model[2], model[0]), cross(model[0], model[1]));
  v_normal = normal_matrix * a_normal;
  gl_Position = u_projection * u_view * world_position;
}
