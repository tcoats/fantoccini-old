#version 330 core

uniform sampler2D shadowMap;
in vec2 uv;

out vec4 fragColor;

void main(){
  fragColor = vec4(texture(shadowMap, uv).xyz, 1.0);
}
