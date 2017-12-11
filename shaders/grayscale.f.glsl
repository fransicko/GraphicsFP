#version 330 core

in vec2 texCoord;

uniform sampler2D fbo;
uniform vec3 location;

out vec4 fragColorOut;

void main() {
    vec4 texel = texture( fbo, texCoord );
    
    vec4 finalColor = texel;
    
    // TODO #A
    
    //float texelSum = texel.r + texel.g + texel.b;
    //texelSum /= 3.0;
    //finalColor = vec4(texelSum,texelSum,texelSum,1);
	finalColor = finalColor + vec4(location.z / 1000.0f, location.z / 1000.0f, location.z / 1000.0f, 0);
    
    fragColorOut = clamp(finalColor, 0.0, 1.0);
	//fragColorOut = texel;
	
	// 2nd effect
	/*if (texCoord.x > 0.495 && texCoord.x < 0.505 && texCoord.y > 0.405 && texCoord.y < 0.595) {
		fragColorOut = vec4(1,0,0,1);
	}
	else if (texCoord.y > 0.495 && texCoord.y < 0.505 && texCoord.x > 0.405 && texCoord.x < 0.595) {
		fragColorOut = vec4(1,0,0,1);
	}
	else if (length(vec2(.5 - texCoord.x, .5 - texCoord.y)) > 0.48) {
		fragColorOut = vec4(0, 0, 0, 1);
	}
	else {
		vec4 newColor = vec4(finalColor.x * 0.1, finalColor.y * 0.95, finalColor.z * 0.2, 1);
		fragColorOut = clamp(newColor, 0.0, 1.0);
	}*/
}
