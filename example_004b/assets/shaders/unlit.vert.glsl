#version 460 core

struct Instance {
  uint transform_index;
  uint color_index;
};

layout (std430, binding = 0) readonly buffer Instances {
  Instance u_instances[];
};

layout (std430, binding = 1) readonly buffer Transforms {
  mat4 u_transforms[];
};

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_uv;

uniform mat4 u_view;
uniform mat4 u_projection;

flat out uint v_color_index;

void main() {
  const Instance instance = u_instances[gl_BaseInstance + gl_InstanceID];
  v_color_index = instance.color_index;
  gl_Position = u_projection * u_view * u_transforms[instance.transform_index] * vec4(a_position, 1.0F);
}
