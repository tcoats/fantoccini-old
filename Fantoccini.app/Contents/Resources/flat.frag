#version 330 core

uniform sampler2DShadow shadowMap;
uniform vec3 lightDirection;
in vec4 vertPosition;
in vec3 vertNormal;
in vec4 vertColor;
in vec4 shadowPosition;

out vec4 fragColor;

vec2 poisson[16] = vec2[](
  vec2(-0.94201624, -0.39906216),
  vec2(0.94558609, -0.76890725),
  vec2(-0.094184101, -0.92938870),
  vec2(0.34495938, 0.29387760),
  vec2(-0.91588581, 0.45771432),
  vec2(-0.81544232, -0.87912464),
  vec2(-0.38277543, 0.27676845),
  vec2(0.97484398, 0.75648379),
  vec2(0.44323325, -0.97511554),
  vec2(0.53742981, -0.47373420),
  vec2(-0.26496911, -0.41893023),
  vec2(0.79197514, 0.19090188),
  vec2(-0.24188840, 0.99706507),
  vec2(-0.81409955, 0.91437590),
  vec2(0.19984126, 0.78641367),
  vec2(0.14383161, -0.14100790)
);

float random(vec3 seed, int i){
  vec4 seed4 = vec4(seed, i);
  float dot_product = dot(seed4, vec4(12.9898, 78.233, 45.164, 94.673));
  return fract(sin(dot_product) * 43758.5453);
}

void main() {
  float ambient = 0.1;
  float bias = 0.005;
  float diffuse = clamp(dot(vertNormal, lightDirection), 0, 1);
  float visibility = 1.0;
  for (int i = 0; i < 4; i++) {
    int index = int(16.0 * random(floor(vertPosition.xyz * 1000.0), i)) % 16;
    visibility -= 0.2 * (1.0 - texture(shadowMap,
      vec3(
        shadowPosition.xy + poisson[index] / 700.0,
        (shadowPosition.z - bias) / shadowPosition.w)));
  }
  // float visibility = texture(
  //   shadowMap, vec3(shadowPosition.xy, shadowPosition.z / shadowPosition.w));
  float brightness = clamp(ambient + visibility * diffuse, 0, 1);
  fragColor = vec4(brightness * vertColor.rgb, vertColor.a);
}
