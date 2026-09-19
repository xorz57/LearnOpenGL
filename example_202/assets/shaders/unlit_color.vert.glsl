#version 460 core

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec3 a_uv;
layout (location = 3) in mat4 a_model;
layout (location = 7) in vec3 a_color;

uniform mat4 u_view;
uniform mat4 u_projection;

out vec3 v_color;

void main() {
  v_color = a_color;
  gl_Position = u_projection * u_view * a_model * vec4(a_position, 1.0F);
}
