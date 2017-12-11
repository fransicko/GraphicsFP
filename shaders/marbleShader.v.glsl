#version 330 core

in vec3 vPosition;
in vec3 normal;
in vec2 vTextureCoord;

out vec2 texCoord;
out vec3 colorAfter;

// Light Properties
uniform vec3 lightPosition;
uniform vec3 lightAmbient;
uniform vec3 lightDiffuse;
uniform vec3 lightSpecular;
uniform float shine;

// Light 2 Properties
uniform vec3 lightPosition2;
uniform vec3 lightAmbient2;
uniform vec3 lightDiffuse2;
uniform vec3 lightSpecular2;
uniform float shine2;

uniform vec3 cameraPosition;

uniform vec3 color;
uniform mat4 modelMtx;
uniform mat4 viewMtx;
uniform mat4 viewProjectionMtx;
uniform mat4 normalMtx;

void main() {
  gl_Position = viewProjectionMtx * modelMtx * vec4(vPosition, 1.0);

  vec4 vPos = viewMtx * modelMtx * vec4(vPosition, 1);
	vec4 lPos = viewMtx * vec4(lightPosition, 1);
	vec4 cPos = viewMtx * vec4(cameraPosition, 1);
	vec4 nPos = normalMtx * vec4(normal, 1);

  vec3 lightVector = normalize(vec3(lPos.x, lPos.y, lPos.z) - vec3(vPos.x, vPos.y, vPos.z));
	vec3 viewVector = normalize(vec3(cPos.x, cPos.y, cPos.z) - vec3(vPos.x, vPos.y, vPos.z));
	vec3 normalVector = normalize(vec3(nPos.x, nPos.y, nPos.z));

	vec3 iDiffuse = lightDiffuse * color * max(0, dot(lightVector, normalVector));
	vec3 iAmbient = lightAmbient * color;
	vec3 iSpecular = lightSpecular * color * pow(max(0, dot(viewVector, normalVector)), shine);

	colorAfter = iDiffuse + iAmbient + iSpecular;
  texCoord = vTextureCoord;

	vec4 lPos2 = viewMtx * vec4(lightPosition2, 1);
	vec3 lightVector2 = normalize(vec3(lPos2.x, lPos2.y, lPos2.z) - vec3(vPos.x, vPos.y, vPos.z));

	vec3 iDiffuse2 = lightDiffuse2 * color * max(0, dot(lightVector2, normalVector));
	vec3 iAmbient2 = lightAmbient2 * color;
	vec3 iSpecular2 = lightSpecular2 * color * pow(max(0, dot(viewVector, normalVector)), shine2);

	colorAfter = colorAfter + iDiffuse2 + iAmbient2 + iSpecular2;
}
