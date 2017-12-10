#version 330 core

in vec2 texCoord;
in vec3 colorAfter;

out vec4 fragColorOut;

uniform sampler2D tex;

void main() {
  fragColorOut = texture( tex, texCoord ) * vec4(colorAfter, 1);
}
