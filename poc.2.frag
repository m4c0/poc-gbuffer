#version 450

layout(input_attachment_index = 0, set = 0, binding = 0) uniform subpassInput i_clr;
layout(input_attachment_index = 1, set = 0, binding = 1) uniform subpassInput i_pos;
layout(input_attachment_index = 2, set = 0, binding = 2) uniform subpassInput i_nrm;

layout(location = 0) out vec4 colour;

void main() {
  colour = subpassLoad(i_clr);
}
