/*
 *   Fragment Shader
 *
 *   CSCI 441, Computer Graphics, Colorado School of Mines
 */

#version 330 core

// TODO #F3
in vec3 theColor;
in vec2 tC;

// TODO #D
out vec4 fragColorOut;

uniform sampler2D tex;

void main() {

    /*****************************************/
    /******* Final Color Calculations ********/
    /*****************************************/
    
    // TODO #E
	//fragColorOut = vec4(1,1,1,1);
	
    // TODO #F4
	fragColorOut = texture( tex, tC ) * vec4(theColor, 1);
}
