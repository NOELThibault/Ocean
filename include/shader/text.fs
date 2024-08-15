#version 330 core

in VS_OUT {
    vec2 texCoords;
    flat int index;
} fs_in;
uniform sampler2DArray tex;
uniform int letterMap[ 200 ];
uniform vec3 textColor;

out vec4 fragColor;

void main()
{
    vec4 sampled = vec4( 1.0, 1.0, 1.0, texture( tex, vec3( fs_in.texCoords, letterMap[ fs_in.index ] ) ).r );
    fragColor = vec4( textColor, 1.0 ) * sampled;
}