#version 460 core

struct Material {
  vec3 diffuse;
  vec3 specular;
  float shininess;
};

layout (std430, binding = 0) readonly buffer Materials {
  Material u_materials[];
};

flat in uint v_material_index;

out vec4 f_color;

void main() {
  f_color = vec4(u_materials[v_material_index].diffuse, 1.0F);
}
