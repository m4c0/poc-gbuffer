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
layout(location = 2) in mat3 f_tbn;

layout(location = 0) out vec4 colour;

void main() {
  vec3 n = texture(ngl, f_uv).rgb * 2.0 - 1.0;
  n = normalize(f_tbn * n);
  vec3 l = normalize(light - f_pos);
  vec3 dif = vec3(1) * dot(n, l);
 
  colour = vec4(dif, 1.0);
  //colour = vec4(n * 0.5 + 0.5, 1.0);
  //colour = vec4(texture(clr, f.uv).rgb, 1.0);
}
