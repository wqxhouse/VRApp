// orthoDepthMap.vert

#version 120

// uniform mat4 u_lightViewMatrix;
uniform float u_nearDistance_s;
uniform float u_zLength;

varying float v_depth;

void main()
{
    vec3 lightViewPosition = vec3(gl_ModelViewMatrix * gl_Vertex);
//    vec3 lightViewPosition = vec3(u_lightViewMatrix * modelMatrix * gl_Vertex);
//    v_depth = (-lightViewPosition.z - u_nearDistance_s) / (u_farDistance_s - u_nearDistance_s);
    v_depth = (-lightViewPosition.z - u_nearDistance_s) / u_zLength;
    
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
