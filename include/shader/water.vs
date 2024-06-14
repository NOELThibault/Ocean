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

vec3 rotate( vec3 v, float angle )
{
    mat3 rotation = mat3( cos( angle ) , 0.0, sin( angle ),
                          0.0          , 1.0, 0.0,
                          -sin( angle ), 0.0, cos( angle ) );
    return rotation * v;
}

// Returns the position and normal of the wave at the given position
mat2x3 wave( vec3 pos )
{
    vec3 newPos = pos;
    float dx = 0.0;
    float dz = 0.0;
    for( int i = 0; i < numWaves; i++ )
    {
        float A = exp( ( float( numWaves ) / float( numWaves + i ) ) * 4.0 - 3.0 ) * 0.15; // Decreasing in interval [ exp(-1); exp(1) ] * 0.5 ~= [ 0.184; 1.359 ] 
        float lambda = float ( numWaves - i ) * 0.5; // Decreasing in interval [ 0.5; numWaves / 2 ]
        vec3 k = 2.0 * PI * normalize( rotate( vec3( 1.0, 0.0, 1.0 ), float( i ) ) ) / lambda; // ||k|| = 2PI / lambda
        float c = 0.75;
        float w = length( k ) * c; // Increasing in interval [ 2PI / numWaves; 2PI ]

        newPos.y += A * cos( w * time + dot( k, pos ) );
        dx += -A * k.x * sin( w * time + dot( k, pos ) );
        dz += -A * k.z * sin( w * time + dot( k, pos ) );
    }
    vec3 normal = normalize( vec3( dx, 1.0, dz ) );
    return mat2x3( newPos, normal );
}

void main()
{
    mat2x3 waveData = wave( aPos );
    vs_out.pos = waveData[ 0 ];
    vs_out.normal = waveData[ 1 ];
    gl_Position = projection * view * vec4( vs_out.pos, 1.0 );
}