#version 120

uniform mat4 u_lightViewMatrix;
uniform mat4 u_lightViewProjectionMatrix;
uniform float u_farDistance;

varying float v_depth;
varying vec3 v_color;

void main()
{
    v_color = gl_FrontColor.rgb;
    vec3 lightViewPosition = vec3(u_lightViewMatrix * gl_Vertex);
    v_depth = -lightViewPosition.z / u_farDistance;
   
    //gl_Position = u_lightViewProjectionMatrix * gl_Vertex;
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}