#version 120
#extension GL_EXT_gpu_shader4 : enable

// a distance bias to reduce the singularity
const float distBias = 0.1f;

varying vec4 v_position;
varying vec4 v_lightPos;
varying vec4 v_lightDir;
varying vec4 v_lightFlux;
varying vec4 v_texcoord; // for deferred shading buffer
varying vec4 v_splatCenter; // for smooth fade outA
varying vec4 v_center2D;

float Ilow = 0.00025f;

vec3 decodeFlux( float h )
{
    vec3 r;
    
    r.z = fract( h / 64.0f / 64.0f );
    r.y = fract( h / 64.0f );
    r.x = fract( h );
    
    return r;
}

// c_precision of 128 fits within 7 base-10 digits
const float c_precision = 128.0;
const float c_precisionp1 = c_precision + 1.0;

/*
 \param color normalized RGB value
 \returns 3-component encoded float
 */
float color2float(vec3 color) {
    color = clamp(color, 0.0, 1.0);
    return floor(color.r * c_precision + 0.5)
    + floor(color.b * c_precision + 0.5) * c_precisionp1
    + floor(color.g * c_precision + 0.5) * c_precisionp1 * c_precisionp1;
}

/*
 \param value 3-component encoded float
 \returns normalized RGB value
 */
vec3 float2color(float value) {
    vec3 color;
    color.r = mod(value, c_precisionp1) / c_precision;
    color.b = mod(floor(value / c_precisionp1), c_precisionp1) / c_precision;
    color.g = floor(value / (c_precisionp1 * c_precisionp1)) / c_precision;
    return color;
}

uniform sampler2D u_samplePosTex;
uniform sampler2DRect u_worldPosTex;
uniform sampler2DRect u_lightDirTex;
uniform sampler2DRect u_sampleTexcoordTex;

uniform mat4 u_matVP;
uniform float u_scaleIndirect;
uniform vec2 u_render_wh;
uniform vec2 u_render_wh_inv;
uniform vec2 u_rsm_wh;
uniform vec3 u_camPos;

varying vec3 fluxDebug;

// varying vec3 v_samplePos;

// uniform vec3 u_arr[256];

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

    // unpack flux
     v_lightFlux.xyz = decodeFlux( v_lightDir.w );
    //v_lightFlux.xyz = float2color(v_lightDir.w);
    
    fluxDebug = v_lightFlux.xyz;
    
    v_lightFlux.w   = 0.0f;
    v_lightFlux    *= u_scaleIndirect;
    
    
//    vec4 offset = vec4(u_arr[gl_InstanceID], 0);
//    vec4 vert = gl_Vertex + offset;
//    gl_Position = gl_ModelViewProjectionMatrix * vert;
//
    // brightness of the pixel light source (RGB luminance)
    float I0 = dot( vec3( 0.3, 0.59, 0.11 ), v_lightFlux.xyz );
    
    // glossiness of the pixel light
    float n = v_lightPos.w;
    v_lightDir.w = n;
    
    // compute ellipsoid (see ShaderX5 for details)
    
    // center
    float c_n = pow( n / ( n + 2.0f ), 0.25f * ( n + 2 ) );
    
    // height/width
    float h_n = pow( c_n, n / ( n + 2.0f ) ) * sqrt( 1.0f - pow( c_n, 4.0f / ( n + 2 ) ) );
    float w_n = max( c_n, 1.0f - c_n );
    
    // construct a tangent space
    vec3 lightDir  = v_lightDir.xyz;
    vec3 tangent   = lightDir.yzx * vec3( -1, 1, 1 );
    vec3 bitangent = cross( lightDir, tangent );
    tangent   = cross( lightDir, bitangent );
    
    // estimate pixel light size
    //float size = sqrt( I0 / Ilow ) * 0.4f;
    float size = sqrt( I0 / Ilow ) * 0.4f * 2.0;
    
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
    
//    // smooth fade out
//    // screen space size of the splat
//    v_lightFlux.w = size + max( w_n, h_n );
//    v_lightFlux.w /= length( v_lightPos.xyz - u_camPos );
//    
//    // and the screen space position of its center
//    v_center2D = u_matVP * vec4( v_lightPos.xyz + size * c_n * lightDir.xyz, 1.0f );
    
    gl_Position = clipPosition;

//    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    
}