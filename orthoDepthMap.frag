#version 120

varying float v_depth;
varying vec4 v_color;

uniform mat4 u_lightViewProjectionMatrix;
varying vec4 v_vertex;

void main()
{
    //gl_FragData[0] = vec4(1, 0, 0, 1);
    gl_FragData[0] = vec4(v_depth, v_depth, v_depth, 1);
}