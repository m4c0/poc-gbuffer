#version 450

layout(set = 0, binding = 0) uniform sampler2D occ;
layout(set = 0, binding = 1) uniform sampler2D clr;
layout(set = 0, binding = 2) uniform sampler2D dsp;
layout(set = 0, binding = 3) uniform sampler2D ndx;
layout(set = 0, binding = 4) uniform sampler2D ngl;
layout(set = 0, binding = 5) uniform sampler2D rgh;

layout(location = 0) in vec2 f_uv;
layout(location = 1) in vec3 f_nrm;

layout(location = 0) out vec4 colour;

void main() {
  colour = vec4(texture(clr, f_uv).rgb, 1.0);
}
