#version 450

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 nrm;

layout(location = 0) out vec2 f_uv;
layout(location = 1) out vec3 f_nrm;

const float fov_rad = radians(90);
const float aspect = 1;

const float f = 1 / atan(fov_rad / 2);
const float far = 10.0;
const float near = 0.001;

const float p22 = (far + near) / (near - far);
const float p32 = (2.0 * far * near) / (near - far);
const mat4 proj = mat4(
  f / aspect, 0.0, 0.0, 0.0,
  0.0, f, 0.0, 0.0,
  0.0, 0.0, p22, p32,
  0.0, 0.0, -1, 0.0
);

void main() {
  gl_Position = vec4(pos.x, -pos.y + 0.25, pos.z - 2, 1) * proj;
  f_uv = uv;
  f_nrm = nrm;
}

