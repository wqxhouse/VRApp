// importanceSampling.vert
#version 120
#extension GL_ARB_shader_texture_lod : enable

#define ADJUST_STEPS	2

uniform sampler2D u_mipMapFlux;
uniform sampler2D u_poissowTex;
uniform vec2 u_mipMapSize;
uniform int u_splatSize;

varying vec2 v_texcoord;

// bias to adjust sampling to RSM size
float globalBias = log( 512.0f / 512.0f ) / log( 2.0f );

// this moves 1 sample tap
vec3 adjustSample( vec3 sampleLocation, vec2 textureOffset, vec2 textureSize, float bias)
{
    if ( sampleLocation.x >= textureOffset.x && sampleLocation.x <= textureOffset.x + textureSize.x &&
         sampleLocation.y >= textureOffset.y && sampleLocation.y <= textureOffset.y + textureSize.y )
    {
        sampleLocation.xy = ( sampleLocation.xy - textureOffset ) / textureSize.y;
        
        vec3 A, B, C, D;
        float  flux1, flux2, flux3, flux4, ratio, fA, fB, fC, fD;
        
        // get the 4 average values of the quadrants
        A = texture2DLod( u_mipMapFlux, vec2( textureOffset + textureSize * vec2( 1, 1 ) * 0.25f), globalBias+bias ).xyz;
        B = texture2DLod( u_mipMapFlux, vec2( textureOffset + textureSize * vec2( 3, 1 ) * 0.25f), globalBias+bias ).xyz;
        C = texture2DLod( u_mipMapFlux, vec2( textureOffset + textureSize * vec2( 1, 3 ) * 0.25f), globalBias+bias ).xyz;
        D = texture2DLod( u_mipMapFlux, vec2( textureOffset + textureSize * vec2( 3, 3 ) * 0.25f), globalBias+bias ).xyz;
        
        // and take average brightness
        fA = dot( vec3( 0.299f, 0.587f, 0.114f ), A );
        fB = dot( vec3( 0.299f, 0.587f, 0.114f ), B );
        fC = dot( vec3( 0.299f, 0.587f, 0.114f ), C );
        fD = dot( vec3( 0.299f, 0.587f, 0.114f ), D );
        
        // adjust sample positions (see ShaderX5 article for details)
        ratio = ( fA + fC ) / ( fA + fB + fC + fD );
        
        if ( sampleLocation.x < ratio )
        {
            sampleLocation.x = sampleLocation.x / ratio * 0.5f;
            sampleLocation.z *= 0.5f / ratio;
            
            ratio = fA / ( fA + fC );
            
            if ( sampleLocation.y < ratio )
            {
                sampleLocation.y = sampleLocation.y / ratio * 0.5f;
                sampleLocation.z *= 0.5f / ratio;
            } else
            {
                sampleLocation.y = 0.5f + ( sampleLocation.y - ratio ) / ( 1.0f - ratio ) * 0.5f;
                sampleLocation.z *= 0.5f / ( 1.0f - ratio );
            }
            
        }
        else
        {
            sampleLocation.x = 0.5f + ( sampleLocation.x - ratio ) / ( 1.0f - ratio ) * 0.5f;
            sampleLocation.z *= 0.5f / ( 1.0f - ratio );
            
            ratio = fB / ( fB + fD );
            
            if ( sampleLocation.y < ratio )
            {
                sampleLocation.y = sampleLocation.y / ratio * 0.5f;
                sampleLocation.z *= 0.5f / ratio;
            }
            else
            {
                sampleLocation.y = 0.5f + ( sampleLocation.y - ratio ) / ( 1.0f - ratio ) * 0.5f;
                sampleLocation.z *= 0.5f / ( 1.0f - ratio );
            }
            
        }
        sampleLocation.xy = sampleLocation.xy * textureSize.x + textureOffset.xy;
    }
    return sampleLocation;
}

void main()
{
    vec2 poi_tex_coord = gl_FragCoord.xy / u_splatSize;
    // get sample location _before_ importance sampling
    vec3 sampleLocation = texture2D( u_poissowTex, poi_tex_coord ).xyz;
    
    // 512² resolution of the RSM => 4 averaged pixels correspond to a lod-bias of 8
    
#if (ADJUST_STEPS>=1)
    sampleLocation = adjustSample( sampleLocation, vec2( 0.0, 0.0 ), vec2( 1.0, 1.0 ), 8);
#endif
    
    // proceed on more quadrants
#if (ADJUST_STEPS>=2)
    sampleLocation = adjustSample( sampleLocation, vec2( 0.0, 0.0 ), vec2( 0.5, 0.5 ), 7 );
    sampleLocation = adjustSample( sampleLocation, vec2( 0.5, 0.0 ), vec2( 0.5, 0.5 ), 7 );
    sampleLocation = adjustSample( sampleLocation, vec2( 0.0, 0.5 ), vec2( 0.5, 0.5 ), 7 );
    sampleLocation = adjustSample( sampleLocation, vec2( 0.5, 0.5 ), vec2( 0.5, 0.5 ), 7 );
#endif
    
    // proceed on even more quadrants
#if (ADJUST_STEPS>=3)
    for ( int j = 0; j < 4; j++ )
        for ( int i = 0; i < 4; i++ )
        {
            sampleLocation = adjustSample( sampleLocation, vec2( 0.25 * i, 0.25 * j ), vec2( 0.25, 0.25 ), 6 );
        }
#endif
    
    // proceed on many many quadrants
#if (ADJUST_STEPS>=4)
    for ( int j = 0; j < 8; j++ )
        for ( int i = 0; i < 8; i++ )
        {
            sampleLocation = adjustSample( sampleLocation, vec2( 0.125 * i, 0.125 * j ), vec2( 0.125, 0.125 ), 5, CBSample );
        }
#endif
    
    // proceed on more quadrants than you want to process ;-)
#if (ADJUST_STEPS>=5)
    for ( int j = 0; j < 16; j++ )
        for ( int i = 0; i < 16; i++ )
        {
            sampleLocation = adjustSample( sampleLocation, vec2( 0.0625 * i, 0.0625 * j ), vec2( 0.0625, 0.0625 ), 4 );
        }
#endif
    
    gl_FragColor = vec4( sampleLocation, 0 );
    
//    vec2 texcoord = vec2(v_texcoord.s / u_mipMapSize.x, v_texcoord.t / u_mipMapSize.y);
//    vec2 texcoord = vec2(gl_FragCoord.x / u_mipMapSize.x, gl_FragCoord.y / u_mipMapSize.y);
//    gl_FragColor = texture2D(u_mipMapFlux, texcoord);
//    vec4 vv = texture2D(u_poissowTex, vec2(gl_FragCoord.x / 64.0, gl_FragCoord.y / 64.0) );
//    gl_FragColor = vv;
//    gl_FragColor = vec4(0, 1, 1, 1);
}