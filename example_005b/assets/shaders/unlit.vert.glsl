#version 460 core

struct Instance {
  uint model_index;
  uint material_index;
};

layout (std430, binding = 0) readonly buffer Instances {
  Instance u_instances[];
};

layout (std430, binding = 1) readonly buffer Models {
  mat4 u_models[];
};

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_uv;

uniform mat4 u_view;
uniform mat4 u_projection;

flat out uint v_material_index;

void main() {
  const Instance instance = u_instances[gl_BaseInstance + gl_InstanceID];
  v_material_index = instance.material_index;
  gl_Position = u_projection * u_view * u_models[instance.model_index] * vec4(a_position, 1.0F);
}
