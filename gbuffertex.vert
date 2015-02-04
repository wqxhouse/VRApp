// Geometry pass with texture vertex shader
#version 120

uniform float u_farDistance;
uniform float u_nearDistance;

varying vec3 v_normal;
varying vec2 v_texCoord;
varying float v_depth;
varying vec4 v_position;

uniform mat4 u_viewMat;
uniform mat4 u_inv_viewMat;
uniform mat4 u_projMat;

varying vec4 v_world;

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
    
    //vec4 viewSpaceVertex = gl_ModelViewMatrix * gl_Vertex;
    mat4 modelView = u_viewMat * getModelToWorldMatrix();
    vec4 viewSpaceVertex = modelView * gl_Vertex;
    v_position = viewSpaceVertex;
    
    //v_normal = gl_NormalMatrix * gl_Normal;
    v_normal = vec3(normalize(modelView * vec4(gl_Normal, 0)));
    v_texCoord = gl_MultiTexCoord1.st; // dae loader puts textureCoords in 1; therefore, force 1 for all
    v_depth = (-viewSpaceVertex.z - u_nearDistance) / (u_farDistance - u_nearDistance);
    
    // gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    
     gl_Position = u_projMat * viewSpaceVertex;
}