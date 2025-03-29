#version 450

layout(location = 0) in vec3 pos;

const float fov_rad = radians(90);
const float aspect = 1;

const float f = 1 / atan(fov_rad / 2);
const float far = 100.0;
const float near = 0.1;

const float p22 = (far + near) / (near - far);
const float p32 = (2.0 * far * near) / (near - far);
const mat4 proj = mat4(
  f / aspect, 0.0, 0.0, 0.0,
  0.0, f, 0.0, 0.0,
  0.0, 0.0, p22, p32,
  0.0, 0.0, -1, 0.0
);

void main() {
  gl_Position = vec4(pos.x, -pos.y, pos.z, 1) * proj;
}

