// fluxMipMap.frag

#version 120

uniform sampler2DRect u_rsmFlux;

vec3 decodeFlux( float h )
{
    vec3 r;
    
    r.z = fract( h / 64.0f / 64.0f );
    r.y = fract( h / 64.0f );
    r.x = fract( h );
    
    return r;
}

void main()
{
    vec4 rt0;
    vec2 texcoord = gl_FragCoord.st;
    
    rt0.xyz = decodeFlux( texture2DRect( u_rsmFlux, texcoord ).w );
    rt0.w   = 0.0f;
    
    gl_FragColor = rt0;
    //gl_FragColor = vec4(1, 1, 0, 1);
}