#version 330 core

in vec3 vPosition;
in vec2 tCoord;

out vec2 tCoord2;

uniform mat4 mvpMatrix;

void main() {

    gl_Position = mvpMatrix * vec4(vPosition, 1);
    
	tCoord2 = tCoord;
}
