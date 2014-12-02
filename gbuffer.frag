// Geometry pass fragment shader
#version 120

varying vec3 v_normal;
varying vec2 v_texCoord;
varying float v_depth;

varying vec4 v_color;
varying vec4 v_position;

void main(void)
{
    gl_FragData[0] = v_color;
    gl_FragData[1] = vec4(normalize(v_normal), v_depth); // normals + depth
    gl_FragData[2] = vec4(v_position.xyz, 0);
}