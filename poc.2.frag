#version 450

layout(input_attachment_index = 0, set = 0, binding = 0) uniform subpassInput i_clr;
layout(input_attachment_index = 1, set = 0, binding = 1) uniform subpassInput i_pos;
layout(input_attachment_index = 2, set = 0, binding = 2) uniform subpassInput i_nrm;

layout(location = 0) out vec4 colour;

const vec3 light = vec3(0, 0, 5);

void main() {
  vec4 c = subpassLoad(i_clr);
  vec3 p = normalize(light - subpassLoad(i_pos).rgb);
  vec3 n = normalize(subpassLoad(i_nrm).rgb);
  float l = clamp(dot(p, n), 0, 1);
  colour = l * c;
}
