#version 450
#extension GL_GOOGLE_include_directive : require
#include "../glslinc/3d.glsl"

layout(push_constant) uniform upc {
  vec3 light;
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
  mat4 proj = projection_matrix(fov_rad, aspect, 0.001, 10.0);

  mat4 model_t = translation(vec3(0, 0.25, -5));
  float a = radians(-45 * sin(time));
  mat4 model_r = mat4(
    1, 0, 0, 0,
    0, cos(a), -sin(a), 0,
    0, sin(a), cos(a), 0,
    0, 0, 0, 1
  );
  mat4 model = model_r * model_t;

  gl_Position = vec4(pos.x, -pos.y, pos.z, 1) * model * proj;
  f_pos = pos;
  f_uv = uv;
  // Object-to-tangent
  f_tbn = mat3(tgt, btgt, nrm);
}

