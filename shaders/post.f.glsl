#version 330 core

in vec2 texCoord;

uniform sampler2D fbo;
uniform vec3 location;

out vec4 fragColorOut;

void main() {
    vec4 texel = texture( fbo, texCoord );
    
    vec4 finalColor = texel;
    
	finalColor = finalColor + vec4(location.z / 1000.0f, location.z / 1000.0f, location.z / 1000.0f, 0);
    
    fragColorOut = clamp(finalColor, 0.0, 1.0);

}
