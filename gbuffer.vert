// Geometry pass vertex shader
#version 120

uniform float u_farDistance;

varying vec3 v_normal;
varying float v_depth;
varying vec4 v_color;
varying vec4 v_position;

void main (void)
{
  vec4 viewSpaceVertex = gl_ModelViewMatrix * gl_Vertex;
  v_position = viewSpaceVertex;
  v_normal = gl_NormalMatrix * gl_Normal;
  v_depth = -viewSpaceVertex.z / u_farDistance; // figure out -z or y
    
  v_color = gl_Color;
  gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}