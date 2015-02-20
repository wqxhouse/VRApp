#version 120

uniform mat4 u_projectionMat;
uniform mat4 u_viewMat;

uniform mat4 osg_ViewMatrixInverse;

// a hack
mat4 getModelToWorldMatrix()
{
    return osg_ViewMatrixInverse * gl_ModelViewMatrix;
}

void main(void)
{
    mat4 modelMatrix = getModelToWorldMatrix();
    mat4 modelViewMatrix = u_viewMat * modelMatrix;
    vec4 view_vertex = modelViewMatrix * gl_Vertex;
    gl_Position = u_projectionMat * view_vertex;
}