// Geometry pass with texture vertex shader
#version 120

uniform float u_farDistance;
uniform float u_nearDistance;

varying vec3 v_normal;
varying vec2 v_texCoord;
varying float v_depth;
varying vec4 v_position;

void main (void)
{
    vec4 viewSpaceVertex = gl_ModelViewMatrix * gl_Vertex;
    v_position = viewSpaceVertex;
    
    v_normal = gl_NormalMatrix * gl_Normal;
    v_texCoord = gl_MultiTexCoord1.st; // dae loader puts textureCoords in 1; therefore, force 1 for all
    v_depth = (-viewSpaceVertex.z - u_nearDistance) / (u_farDistance - u_nearDistance);
    
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}