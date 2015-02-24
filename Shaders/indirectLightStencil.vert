// indirect Ligth Stencil vertex shader
// TODO: find another way to do vpl culling since this is too expensive

void main()
{
    vec3 samplePos;
    
    // get sample position
    vec2 sampleTextureCoord = texture2DRect(u_sampleTexcoordTex, vec2(gl_InstanceID, 0)).st;
    samplePos = texture2D( u_samplePosTex, sampleTextureCoord ).xyz;
    
    // and use it to retrieve the data from the RSM
    // position + phong exponent
    vec2 sampleRectCoord = samplePos.st * u_rsm_wh;
    v_lightPos = texture2DRect( u_worldPosTex, sampleRectCoord );
    
    // main light direction + squeezed flux
    v_lightDir = texture2DRect( u_lightDirTex, sampleRectCoord );

    // brightness of the pixel light source (RGB luminance)
    float I0 = dot( vec3( 0.3, 0.59, 0.11 ), v_lightFlux.xyz );
    
    // glossiness of the pixel light
    float n = v_lightPos.w;
    v_lightDir.w = n;
    
    // center
    float c_n = pow( n / ( n + 2.0f ), 0.25f * ( n + 2 ) );
    
    // height/width
    float h_n = pow( c_n, n / ( n + 2.0f ) ) * sqrt( 1.0f - pow( c_n, 4.0f / ( n + 2 ) ) );
    float w_n = max( c_n, 1.0f - c_n );
    
    // construct a tangent space
    vec3 lightDir  = v_lightDir.xyz;
    vec3 tangent   = lightDir.yzx * vec3( -1, 1, 1 );
    vec3 bitangent = cross( lightDir, tangent );
    tangent = cross( lightDir, bitangent );
    
    float size = sqrt( I0 / Ilow ) * 0.4f;
    
    // scale tangent space by ellipsoid approximation
    lightDir  *= w_n;
    tangent   *= h_n;
    bitangent *= h_n;
    
    // and transform vertex of bounding geometry
    vec4 worldPos;
    
    worldPos.xyz  = gl_Vertex.y * lightDir.xyz;
    worldPos.xyz += gl_Vertex.x * tangent.xyz;
    worldPos.xyz += gl_Vertex.z * bitangent.xyz;
    
    worldPos.xyz += c_n * lightDir.xyz;
    worldPos.xyz *= size;
    
    worldPos.xyz += v_lightPos.xyz;
    worldPos.w    = 1.0f;
    
    vec4 clipPosition = u_matVP * worldPos;
    
    // compute screen space position for deferred shading
    vec4 pos2D = clipPosition;
    
    gl_Position = clipPosition;
}

