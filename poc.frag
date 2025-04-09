#version 450

layout(push_constant) uniform upc {
  vec3 light;
};

layout(set = 0, binding = 0) uniform sampler2D occ;
layout(set = 0, binding = 1) uniform sampler2D clr;
layout(set = 0, binding = 2) uniform sampler2D dsp;
layout(set = 0, binding = 3) uniform sampler2D ndx;
layout(set = 0, binding = 4) uniform sampler2D ngl;
layout(set = 0, binding = 5) uniform sampler2D rgh;

layout(location = 0) in vec3 f_pos;
layout(location = 1) in vec2 f_uv;
layout(location = 2) in vec3 f_nrm;
layout(location = 3) in mat3 f_tbn;

layout(location = 0) out vec4 colour;

void main() {
  // vec3 n = texture(ngl, f_uv).rgb * 2.0 - 1.0;
  vec3 n = f_nrm;
  vec3 l = normalize(light - f_pos); // * f_tbn;
  vec3 dif = vec3(1) * clamp(dot(n, l), 0, 1);
 
  colour = vec4(dif, 1.0);
}
