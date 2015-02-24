// orthoDepthMap.vert

#version 120

uniform mat4 u_lightViewMatrix;
// uniform mat4 u_lightViewProjectionMatrix;
// uniform float u_farDistance_s;
uniform float u_nearDistance_s;
uniform float u_zLength;

// uniform mat4 u_lightProjectionMatrix;
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
//    vec3 lightViewPosition = vec3(u_lightViewMatrix * modelMatrix * gl_Vertex);
//    v_depth = (-lightViewPosition.z - u_nearDistance_s) / (u_farDistance_s - u_nearDistance_s);
    v_depth = (-lightViewPosition.z - u_nearDistance_s) / u_zLength;
    // v_normal = vec3(normalize(gl_ModelViewMatrix * vec4(gl_Normal.xyz, 0)));
    
    v_worldNormal = vec3(modelMatrix * vec4(gl_Normal.xyz, 0));
    v_worldPosition = vec3(modelMatrix * gl_Vertex);
    
    v_color = gl_Color;
    //gl_Position = u_cam_projection * u_cam_view * m_to_w_mat * gl_Vertex;
    
    // TODO: figure out a way to get out model matrix to avoid using built-in modelviewprojection matrix
    // using the built-in one will make this off-by-one-frame, causing flickering when light moves
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
