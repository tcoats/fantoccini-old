#version 330 core

uniform mat4 worldShadowTransform;
in mat4 modelTransform;
in vec3 position;

void main() {
  gl_Position = worldShadowTransform * modelTransform * vec4(position, 1.0);
}
