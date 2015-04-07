// orthoDepthMap.vert

#version 120

uniform mat4 u_lightViewInverseMatrix;

varying float v_depth;
varying vec4 v_color;

varying vec3 v_worldNormal;
varying vec3 v_worldPosition;

// built-in
uniform mat4 osg_ViewMatrixInverse; // seems like this is always the inverse of the mainCamera

// a hack
mat4 getModelToWorldMatrix()
{
    return u_lightViewInverseMatrix * gl_ModelViewMatrix;
}

// TODO: texture mapping
void main()
{
    mat4 modelMatrix = getModelToWorldMatrix();
    
    vec3 lightViewPosition = vec3(gl_ModelViewMatrix * gl_Vertex);
    
    v_worldNormal = vec3(modelMatrix * vec4(gl_Normal.xyz, 0));
    v_worldPosition = vec3(modelMatrix * gl_Vertex);
    
    v_color = gl_Color;
    
    // TODO: figure out a way to get out model matrix to avoid using built-in modelviewprojection matrix
    // using the built-in one will make this off-by-one-frame, causing flickering when light moves
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}