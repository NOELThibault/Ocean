#version 330 core

in VS_OUT {
    vec3 pos;
    vec3 normal;
} fs_in;
uniform vec3 viewPos;

out vec4 fragColor;

void main()
{
    vec3 color = vec3( 0.0, 0.1, 1.0 );
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
    vec3 specular = vec3( 0.4 ) * spec;

    float gamma = 2.2;
    // fragColor = vec4( fs_in.normal, 1.0 );
    fragColor = vec4( pow( ambient + diffuse + specular, vec3( 1 / gamma ) ), 1.0 );
}