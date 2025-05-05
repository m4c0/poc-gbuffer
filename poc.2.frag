#version 450

layout(push_constant) uniform upc {
  vec4 cam;
};

layout(input_attachment_index = 0, set = 0, binding = 0) uniform subpassInput i_clr;
layout(input_attachment_index = 1, set = 0, binding = 1) uniform subpassInput i_pos;
layout(input_attachment_index = 2, set = 0, binding = 2) uniform subpassInput i_nrm;
layout(input_attachment_index = 3, set = 0, binding = 3) uniform subpassInput i_rox;

layout(location = 0) out vec4 colour;

const vec3 light = vec3(90, 0, 0);
const vec3 light_colour = vec3(1, 1, 0.9);
const vec3 ambient = vec3(0, 0, 0.01);

void main() {
  vec4 rox = subpassLoad(i_rox);
  float rgh = rox.r * 32;
  float occ = rox.g;

  vec4 c = subpassLoad(i_clr);
  vec3 n = normalize(subpassLoad(i_nrm).rgb);
  vec3 p = subpassLoad(i_pos).rgb;

  vec3 lp = normalize(light - p);
  vec3 diffuse = occ * clamp(dot(n, lp), 0, 1) * c.rgb;

  vec3 vp = normalize(cam.xyz - p);
  vec3 rn = reflect(-lp, n);
  float s = pow(max(0, dot(vp, rn)), rgh);
  vec3 specular = occ * s * light_colour; // TODO: take 0.8 from material

  colour = vec4(specular + diffuse + ambient, 1);
  //colour = vec4(n * 0.5 + 0.5, 1);
}
