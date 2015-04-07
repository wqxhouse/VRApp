#version 120

uniform mat4 u_viewProjMat;
varying vec3 TexCoord0;

// built-in
uniform mat4 osg_ViewMatrixInverse;

// a hack
mat4 getModelToWorldMatrix()
{
    return osg_ViewMatrixInverse * gl_ModelViewMatrix;
}

void main()
{
    mat4 modelMat = getModelToWorldMatrix();
    vec4 WVP_Pos =  u_viewProjMat * modelMat * gl_Vertex;
    gl_Position = WVP_Pos.xyww;
    TexCoord0 = gl_Vertex.xyz;
}
