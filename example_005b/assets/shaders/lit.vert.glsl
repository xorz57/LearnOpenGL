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

out vec3 v_position;
out vec3 v_normal;

flat out uint v_material_index;

void main() {
  const Instance instance = u_instances[gl_BaseInstance + gl_InstanceID];
  const mat4 model = u_models[instance.model_index];
  const vec4 world_position = model * vec4(a_position, 1.0F);
  v_position = world_position.xyz;
  v_normal = transpose(inverse(mat3(model))) * a_normal;
  v_material_index = instance.material_index;
  gl_Position = u_projection * u_view * world_position;
}
