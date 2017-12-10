#version 330 core

in vec2 tCoord2;

out vec4 fragColorOut;

uniform sampler2D textureMap;

void main() {
    
    vec4 texel = texture(textureMap, tCoord2);
	
	//fragColorOut = vec4(tCoord2,0,1);
    fragColorOut = texel;
}
