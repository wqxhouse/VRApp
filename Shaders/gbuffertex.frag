// Geometry pass with texture fragment shader
#version 120

uniform sampler2D u_texture;

varying vec3 v_normal;
varying vec2 v_texCoord;
varying float v_depth;

varying vec4 v_color;
varying vec4 v_position;

varying vec4 v_world;

void main(void)
{
    vec3 diffuse = texture2D(u_texture, v_texCoord).rgb;
    gl_FragData[0] = vec4(diffuse, 1.0); // albedo
    gl_FragData[1] = vec4(normalize(v_normal), v_depth); // normals + depth
    gl_FragData[2] = vec4(v_position.xyz, 0);
}