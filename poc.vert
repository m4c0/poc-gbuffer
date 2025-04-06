#version 450

layout(push_constant) uniform upc {
  vec3 light;
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
const float aspect = 1;

const float fp = 1 / atan(fov_rad / 2);
const float far = 10.0;
const float near = 0.001;

const float p22 = (far + near) / (near - far);
const float p32 = (2.0 * far * near) / (near - far);
const mat4 proj = mat4(
  fp / aspect, 0.0, 0.0, 0.0,
  0.0, fp, 0.0, 0.0,
  0.0, 0.0, p22, p32,
  0.0, 0.0, -1, 0.0
);

void main() {
  float a = radians(-45);
  mat4 model_t = mat4(
    1, 0, 0, 0,
    0, 1, 0, 0.25,
    0, 0, 1, -2,
    0, 0, 0, 1
  );
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
  f_tbn = mat3(
    normalize(vec3(model * vec4(tgt,  0.0))),
    normalize(vec3(model * vec4(btgt, 0.0))),
    normalize(vec3(model * vec4(nrm,  0.0)))
  );
}

