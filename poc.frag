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

layout(location = 0) in struct {
  vec3 pos;
  vec3 nrm;
  vec2 uv;
  mat3 tbn;
} f;

layout(location = 0) out vec4 colour;

void main() {
  vec3 l = normalize(light - f.pos);
  vec3 n = normalize(f.nrm);
  vec3 dif = vec3(1) * dot(n, l);
 
  colour = vec4(dif, 1.0);
}
