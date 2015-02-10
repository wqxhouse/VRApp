// point light vertex shader
#version 120

varying vec4 v_vertex;

uniform mat4 u_projMatrix;
uniform mat4 u_viewMatrix;

uniform mat4 osg_ViewMatrixInverse;

// a hack
mat4 getModelToWorldMatrix()
{
    return osg_ViewMatrixInverse * gl_ModelViewMatrix;
}

void main(void)
{
    // v_vertex = gl_ModelViewMatrix * gl_Vertex;
    
    mat4 modelMatrix = getModelToWorldMatrix();
    mat4 modelViewMatrix = u_viewMatrix * modelMatrix;
    v_vertex = modelViewMatrix * gl_Vertex;
    
    // using self supplied matrices from _mainCamera also fixes the z-depth calculation... damn osg is buggy!?
    gl_Position = u_projMatrix * v_vertex;
    //gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
