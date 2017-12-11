/*
 *   Vertex Shader
 *
 *   CSCI 441, Computer Graphics, Colorado School of Mines
 */

#version 330 core

// TODO #B
in vec3 vPosition;
in vec3 normal;
in vec2 tCoord;

// TODO #F1
out vec3 theColor;

// TODO #A
uniform mat4 mvpMatrix;

// Material Properties
uniform vec3 ambient;
uniform vec3 diffuse;
uniform vec3 specular;

// Light Properties
uniform vec3 lightPos;
uniform vec3 lightambient;
uniform vec3 lightdiffuse;
uniform vec3 lightspecular;
uniform float shine;

// Light 2 Properties
uniform vec3 lightPos2;
uniform vec3 lightambient2;
uniform vec3 lightdiffuse2;
uniform vec3 lightspecular2;
uniform float shine2;

uniform mat4 modelMtx;
uniform mat4 viewMtx;
uniform mat4 normMtx;

// Camera Properties
uniform vec3 cameraPos;

// TODO #G1


void main() {
    /*****************************************/
    /********* Vertex Calculations  **********/
    /*****************************************/
    
	gl_Position = mvpMatrix * vec4(vPosition, 1);
	
	vec4 vPos = viewMtx * modelMtx * vec4(vPosition, 1);
	vec4 lPos = viewMtx * vec4(lightPos, 1);
	vec4 cPos = viewMtx * vec4(cameraPos, 1);
	vec4 nPos = normMtx * vec4(normal, 1);
	
	vec3 lightVector = normalize(vec3(lPos.x, lPos.y, lPos.z) - vec3(vPos.x, vPos.y, vPos.z));
	vec3 viewVector = normalize(vec3(cPos.x, cPos.y, cPos.z) - vec3(vPos.x, vPos.y, vPos.z));
	vec3 normalVector = normalize(vec3(nPos.x, nPos.y, nPos.z));

	vec3 iDiffuse = lightdiffuse * diffuse * max(0, dot(lightVector, normalVector));
	vec3 iAmbient = lightambient * ambient;
	vec3 iSpecular = lightspecular * specular * pow(max(0, dot(viewVector, normalVector)), shine);

	theColor = iDiffuse + iAmbient + iSpecular;

	vec4 lPos2 = viewMtx * vec4(lightPos2, 1);
	vec3 lightVector2 = normalize(vec3(lPos2.x, lPos2.y, lPos2.z) - vec3(vPos.x, vPos.y, vPos.z));

	vec3 iDiffuse2 = lightdiffuse2 * diffuse * max(0, dot(lightVector2, normalVector));
	vec3 iAmbient2 = lightambient2 * ambient;
	vec3 iSpecular2 = lightspecular2 * specular * pow(max(0, dot(viewVector, normalVector)), shine2);

	theColor = theColor + iDiffuse2 + iAmbient2 + iSpecular2;
}
