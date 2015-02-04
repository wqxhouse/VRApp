// orthoDepthMap.vert

#version 120

uniform mat4 u_lightViewMatrix;
uniform mat4 u_lightViewProjectionMatrix;
uniform float u_farDistance_s;
uniform float u_nearDistance_s;

uniform mat4 u_lightProjectionMatrix;
uniform mat4 u_lightViewInverseMatrix;

varying float v_depth;
varying vec3 v_position;
varying vec3 v_normal;
varying vec4 v_color;

varying vec3 v_worldPosition;

// built-in
uniform mat4 osg_ViewMatrixInverse;

// a hack
mat4 getModelToWorldMatrix()
{
    return u_lightViewInverseMatrix * gl_ModelViewMatrix;
}

// TODO: texture mapping
void main()
{
    vec3 lightViewPosition = vec3(gl_ModelViewMatrix * gl_Vertex);
    v_depth = (-lightViewPosition.z - u_nearDistance_s) / (u_farDistance_s - u_nearDistance_s);
    v_position = lightViewPosition;
    v_normal = vec3(normalize(lightMV * vec4(gl_Normal.xyz, 0)));
    v_color = gl_Color;
    v_worldPosition = gl_Vertex.xyz;
   
    v_v = lightViewPosition;
    
    //gl_Position = u_cam_projection * u_cam_view * m_to_w_mat * gl_Vertex;
    
    // TODO: figure out a way to get out model matrix to avoid using built-in modelviewprojection matrix
    // using the built-in one will make this off-by-one-frame, causing flickering when light moves
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
