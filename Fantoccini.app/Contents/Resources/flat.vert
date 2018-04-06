#version 330 core

uniform mat4 worldTransform;
uniform mat4 shadowBiasTransform;
in mat4 modelTransform;
in vec3 position;
in vec3 normal;
in vec4 color;

out vec4 vertPosition;
out vec3 vertNormal;
out vec4 vertColor;
out vec4 shadowPosition;

void main() {
  vec4 modelPosition = modelTransform * vec4(position, 1.0);
  vertPosition = worldTransform * modelPosition;
  vertNormal = normalize(mat3(inverse(transpose(modelTransform))) * normal);
  vertColor = color;
  shadowPosition = shadowBiasTransform * modelPosition;
  gl_Position = vertPosition;
}
