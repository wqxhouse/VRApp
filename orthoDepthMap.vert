#version 120

uniform mat4 u_lightViewMatrix;
uniform mat4 u_lightViewProjectionMatrix;
uniform float u_farDistance;
uniform float u_nearDistance;

varying float v_depth;
varying vec4 v_color;
varying vec4 v_vertex;

void main()
{
    v_vertex = gl_Vertex;
    v_color = gl_Color;
    vec3 lightViewPosition = vec3(u_lightViewMatrix * gl_Vertex);
    v_depth = (-lightViewPosition.z - u_nearDistance) / (u_farDistance - u_nearDistance);
   
    gl_Position = u_lightViewProjectionMatrix * gl_Vertex;
}