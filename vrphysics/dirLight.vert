// Directional Light vertex shader
#version 120

uniform mat4 u_viewMatrixInverse;

void main(void)
{
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}