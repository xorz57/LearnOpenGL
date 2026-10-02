#version 460 core

struct Light {
  vec3 position;
  vec3 color;
  float intensity;
};

struct Material {
  vec3 diffuse;
  vec3 specular;
  float shininess;
};

layout (std430, binding = 2) readonly buffer Materials {
  Material u_materials[];
};

uniform vec3 u_ambient_light;
uniform Light u_light;
uniform vec3 u_view_position;

in vec3 v_position;
in vec3 v_normal;

flat in uint v_material_index;

out vec4 f_color;

void main() {
  const vec3 normal = normalize(v_normal);
  const vec3 light_direction = normalize(u_light.position - v_position);
  const vec3 view_direction = normalize(u_view_position - v_position);
  const vec3 halfway_direction = normalize(light_direction + view_direction);

  const Material material = u_materials[v_material_index];

  const vec3 radiance = u_light.color * u_light.intensity;
  const float diffuse_factor = max(dot(normal, light_direction), 0.0F);
  const float specular_factor = pow(max(dot(normal, halfway_direction), 0.0F), material.shininess);

  const vec3 ambient = u_ambient_light * material.diffuse;
  const vec3 diffuse = diffuse_factor * radiance * material.diffuse;
  const vec3 specular = specular_factor * radiance * material.specular;

  f_color = vec4(ambient + diffuse + specular, 1.0F);
}
