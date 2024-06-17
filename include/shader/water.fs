#version 330 core

in VS_OUT {
    vec3 pos;
    vec3 normal;
} fs_in;
uniform vec3 viewPos;
uniform samplerCube reflectionTexture;
uniform vec3 fogColor;
uniform float fogStart;
uniform float fogEnd;
uniform float gamma;
uniform float ambientStrength;
uniform float shininess;
uniform float fresnelStrength;

out vec4 fragColor;

vec3 fog( vec3 color, float depth )
{
    float factor = smoothstep( fogStart, fogEnd, depth );
    return mix( color, fogColor, factor );
}

void main()
{
    vec3 color = vec3( 0.0, 0.15, 1.0 );
    // Ambient
    vec3 ambient = color * 0.1;
    // Diffuse
    vec3 lightDir = normalize( vec3( -1.0, 1.0, -1.0 ) );
    float diff = max( dot( fs_in.normal, lightDir ), 0.0 );
    vec3 diffuse = color * diff;
    // Specular
    float shininess = 64.0;
    vec3 reflectDir = reflect( -lightDir, fs_in.normal );
    vec3 viewDir = normalize( viewPos - fs_in.pos );
    vec3 halfwayDir = normalize( lightDir + viewDir );
    float spec = pow( max( dot( fs_in.normal, halfwayDir ), 0.0 ), shininess );
    float fresnel = 0.02 + ( 1.0 - 0.02 ) * fresnelStrength * pow( 1.0 - clamp( dot( viewDir, fs_in.normal ), 0.0, 1.0 ), 5.0 );
    vec3 specular = vec3( 0.4 ) * spec * fresnel;

    vec3 reflectColor = texture( reflectionTexture, reflectDir ).rgb;

    vec3 rgb = mix( ambient + diffuse + specular, reflectColor, fresnel );
    rgb = fog( rgb, length( viewPos - fs_in.pos ) );
    fragColor = vec4( pow( rgb, vec3( 1 / gamma ) ), 1.0 );
}