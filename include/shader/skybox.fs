#version 330 core

in vec3 texCoords;
uniform samplerCube skybox;
uniform vec3 fogColor;
uniform float fogHeight;

out vec4 fragColor;

void main()
{
    float fogFactor = clamp( ( fogHeight - texCoords.y ) / fogHeight, 0.0, 1.0 );
    vec3 color = mix( texture( skybox, texCoords ).rgb, fogColor, fogFactor );
    
    float gamma = 2.2;
    fragColor = vec4( pow( color, vec3( 1.0 / gamma ) ), 1.0 );
}