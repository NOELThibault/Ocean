#version 330 core
layout ( location = 0 ) in vec2 vertex;

uniform mat4 projection;
uniform mat4 transforms[ 200 ];

out VS_OUT
{
    vec2 texCoords;
    flat int index;
} vs_out;

void main()
{
    gl_Position = projection * transforms[ gl_InstanceID ] * vec4( vertex, 0.0, 1.0 );
    vs_out.texCoords = vertex;
    vs_out.texCoords.y = 1.0 - vertex.y; // tricks from the order of the vertices
    vs_out.index = gl_InstanceID;
}