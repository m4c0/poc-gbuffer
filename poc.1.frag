#version 450

layout(set = 0, binding = 0) uniform sampler2D occ;
layout(set = 0, binding = 1) uniform sampler2D clr;
layout(set = 0, binding = 2) uniform sampler2D dsp;
layout(set = 0, binding = 3) uniform sampler2D ndx;
layout(set = 0, binding = 4) uniform sampler2D ngl;
layout(set = 0, binding = 5) uniform sampler2D rgh;

layout(location = 0) in vec3 f_pos;
layout(location = 1) in vec2 f_uv;
layout(location = 2) in mat3 f_tbn;

layout(location = 0) out vec4 o_clr;
layout(location = 1) out vec4 o_pos;
layout(location = 2) out vec4 o_nrm;
layout(location = 3) out vec4 o_rox;

void main() {
  vec3 n = texture(ngl, f_uv).rgb * 2.0 - 1.0;
  vec4 r = texture(rgh, f_uv);
  vec4 o = texture(occ, f_uv);
 
  o_clr = texture(clr, f_uv);
  o_pos = vec4(f_pos, 0);
  o_nrm = vec4(f_tbn * n, 0);
  o_rox = vec4(r.r, o.r, 0, 0);
}
