#version 450
#extension GL_GOOGLE_include_directive : require
#include "../glslinc/3d.glsl"

layout(push_constant) uniform upc {
  vec4 cam;
  float aspect;
  float time;
};

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 nrm;
layout(location = 3) in vec3 tgt;
layout(location = 4) in vec3 btgt;

layout(location = 0) out vec3 f_pos;
layout(location = 1) out vec2 f_uv;
layout(location = 2) out mat3 f_tbn;

const float fov_rad = radians(90);

void main() {
  mat4 view = view_matrix(cam.xyz, cam.w, vec3(0, 1, 0));
  mat4 proj = projection_matrix(fov_rad, aspect, 0.001, 100.0);

  float a = radians(-45 * time);
  mat4 model = mat4(
    cos(a), 0, -sin(a), 0,
    0, 1, 0, 0,
    sin(a), 0, cos(a), 0,
    0, 0, 0, 1
  );
  vec4 p = vec4(pos, 1) * model;

  gl_Position = vec4(1, -1, 1, 1) * p * view * proj;
  f_pos = p.xyz;
  f_uv = uv;
  f_tbn = mat3(
    normalize(vec3(vec4(tgt,  0) * model)),
    normalize(vec3(vec4(btgt, 0) * model)),
    normalize(vec3(vec4(nrm,  0) * model))
  );
}

