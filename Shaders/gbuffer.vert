// Geometry pass vertex shader
#version 120

uniform float u_farDistance;
uniform float u_nearDistance;

varying vec3 v_normal;
varying float v_depth;
varying vec4 v_color;
varying vec4 v_position;

uniform mat4 u_viewMat;
uniform mat4 u_inv_viewMat;
uniform mat4 u_projMat;

varying vec4 v_world;

//varying vec4 ras;

// built-in
uniform mat4 osg_ViewMatrixInverse;

// a hack
mat4 getModelToWorldMatrix()
{
    return osg_ViewMatrixInverse * gl_ModelViewMatrix;
}

void main (void)
{
    v_world = gl_Vertex;
    
    // root of evil gl_ModelViewMatrix ...
    mat4 modelView = u_viewMat * getModelToWorldMatrix();
    vec4 viewSpaceVertex = modelView * gl_Vertex;
    v_position = viewSpaceVertex;
//    v_position = getModelToWorldMatrix() * v_world;
    
    //  v_normal = gl_NormalMatrix * gl_Normal;
   // v_normal = vec3(getModelToWorldMatrix() * vec4(gl_Normal, 0));
    v_normal = vec3(modelView * vec4(gl_Normal.xyz, 0));
    v_depth = (-viewSpaceVertex.z - u_nearDistance) / (u_farDistance - u_nearDistance);
    
    v_color = gl_Color;
    //gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    
    gl_Position = u_projMat * viewSpaceVertex;
    //ras = gl_Position;
}