#version 330 core

uniform mat4 worldDepthTransform;
in mat4 modelTransform;
in vec3 position;

void main() {
  gl_Position = worldDepthTransform * modelTransform * vec4(position, 1.0);
}

// #version 330 core

// uniform mat4 worldDepthTransform;
// in mat4 modelTransform;
// in vec3 position;

// void main() {
//   gl_Position = worldDepthTransform * modelTransform * vec4(position, 1.0);
// }
