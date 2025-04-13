#version 450

layout(input_attachment_index = 0, set = 0, binding = 0) uniform subpassInput i_clr;

layout(location = 0) out vec4 colour;

void main() {
  colour = subpassLoad(i_clr);
}
