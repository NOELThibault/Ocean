#version 330 core
layout ( location = 0 ) in vec3 aPos;

uniform mat4 view;
uniform mat4 projection;
uniform int numWaves;
uniform float time;
#define PI 3.14159265359

out VS_OUT
{
    vec3 pos;
    vec3 normal;
} vs_out;

float random( float seed )
{
    return fract( sin( seed * 12.9898 ) * 43758.5453 );
}

// Returns the position and normal of the wave at the given position
mat2x3 wave( vec3 pos )
{
    vec3 newPos = pos;
    float dx = 0.0;
    float dz = 0.0;
    vec3 normal = vec3( 0.0, 1.0, 0.0 );

    float A = 1.0;
    vec3 k = vec3( 1.0, 0.0, 0.0 );
    float c = 2.0 * PI / 3.0;
    float w = 1.0;
    for( int i = 0; i < numWaves; i++ )
    {
        float d = dot( k, newPos );
        float f = A * exp( cos( c * time + w * d ) - 1 );
        float fp = -w * sin( c * time + w * d ) * f;

        newPos.y += f;
        dx += k.x * fp;
        dz += k.z * fp;
        normal += vec3( dx, 1.0, dz );

        A *= 0.82;
        k = normalize( vec3( random( float( i ) ), 0.0, 1.0 - random( float( i ) ) ) ) * 0.5;
        w *= 1.18;
    }
    normal = normalize( normal );
    return mat2x3( newPos, normal );
}

void main()
{
    mat2x3 waveData = wave( aPos );
    vs_out.pos = waveData[ 0 ];
    vs_out.normal = waveData[ 1 ];
    gl_Position = projection * view * vec4( vs_out.pos, 1.0 );
}