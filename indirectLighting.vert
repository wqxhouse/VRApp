#version 120

// a distance bias to reduce the singularity
const half distBias = 0.1f;

varying vec4 v_position;
varying vec4 v_lightPos;
varying vec4 v_lightDir;
varying vec4 v_lightFlux;
varying vec4 v_texcoord; // for deferred shading buffer
varying vec4 v_splatCenter; // for smooth fade out

float Ilow = 0.005f;

vec3 decodeFlux( float h )
{
    vec3 r;
    
    r.z = fract( h / 64.0f / 64.0f );
    r.y = fract( h / 64.0f );
    r.x = fract( h );
    
    return r;
}

uniform sampler2D u_samplePosTex;
uniform sampler2DRect u_worldPosTex;
uniform sampler2DRect u_lightDirTex;

uniform mat4 u_matVP;
uniform float u_scaleIndirect;
uniform vec2 u_render_wh;
uniform vec3 u_camPos;

void main()
{
    vec3 samplePos;
    
    // get sample position
    samplePos = texture2D( u_samplePosTex, vec4( vertex.texcoord.xy, 0, 0 ) );
    
    // and use it to retrieve the data from the RSM
    
    // position + phong exponent
    v_lightPos = texture2DRect( u_worldPosTex, samplePos.xy );
    
    // main light direction + squeezed flux
    v_lightDir = texture2DRect( u_lightDirTex, samplePos.xy );
    
    // unpack flux
    v_lightFlux.xyz = decodeFlux( result.lightDir.w );
    v_lightFlux.w   = 0.0f;
    v_lightFlux    *= u_scaleIndirect;
    
    // brightness of the pixel light source
    float I0 = dot( vec3( 0.3, 0.59, 0.11 ), v_lightFlux );
    
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
    vec3 lightDir  = v_lightDir;
    vec3 tangent   = lightDir.yzx * vec3( -1, 1, 1 );
    vec3 bitangent = cross( lightDir, tangent );
    tangent   = cross( lightDir, bitangent );
    
    // estimate pixel light size (and scale according our global scene scale)
//    float size = sqrt( I0 / Ilow ) * 0.4f * WSSIZE;
    float size = sqrt( I0 / Ilow ) * 0.4f;
    
    // scale tangent space by ellipsoid approximation
    lightDir  *= w_n;
    tangent   *= h_n;
    bitangent *= h_n;
    
    // and transform vertex of bounding geometry
    vec4 worldPos;
    
    worldPos.xyz  = vertex.position.y * lightDir.xyz;
    worldPos.xyz += vertex.position.x * tangent.xyz;
    worldPos.xyz += vertex.position.z * bitangent.xyz;
    
    worldPos.xyz += c_n * lightDir.xyz;
    worldPos.xyz *= size;
    
    worldPos.xyz += result.lightPos;
    worldPos.w    = 1.0f;
   
    vec4 clipPosition = u_matVP * worldPos
    
    // compute screen space position for deferred shading
    vec4 pos2D = clipPosition;
    
    // [-1;1] -> [0;1]
    pos2D.xy  = pos2D.xy * vec2( 0.5f, -0.5f ) + 0.5f * pos2D.w;
    // sample texel centers
    pos2D.xy += vec2( 0.5f, 0.5f ) * u_render_wh * pos2D.w;
    
    v_texcoord = pos2D;
    
    // smooth fade out
    // screen space size of the splat
    v_lightFlux.w = size + max( w_n, h_n );
    v_lightFlux.w /= length( v_lightPos.xyz - u_camPos );
    
    // and the screen space position of its center
    v_center2D = u_matVP * vec4( u_lightPos.xyz + size * c_n * lightDir.xyz, 1.0f );
    v_center2D.xy = v_center2D.xy * vec2( 0.5f, -0.5f ) + 0.5f * v_center2D.w;
    
    gl_Position = clipPosition;
}