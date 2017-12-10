/*
 *   Vertex Shader
 *
 *   CSCI 441, Computer Graphics, Colorado School of Mines
 */

#version 330 core

// TODO #B
in vec3 vPosition;

// TODO #F1
out vec3 theColor;

// TODO #A
uniform mat4 mvpMatrix;

void main() {
	gl_Position = mvpMatrix * vec4(vPosition, 1);
    
	theColor = vPosition;
}
