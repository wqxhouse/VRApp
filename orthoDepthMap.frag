// orthoDepthMap.frag

#version 120

varying float v_depth;
varying vec4 v_color;

varying vec3 v_worldPosition;
varying vec3 v_worldNormal;

uniform mat4 u_lightViewProjectionMatrix;
uniform vec3 u_lightPos;

float encodeFlux(vec3 flux)
{
    flux = clamp(flux, 0.0, 1.0);
    float squeezedFlux = flux.x +
        floor( flux.y * 64.0f ) +
        floor( flux.z * 64.0f ) * 64.0f;
    
    return squeezedFlux;
}

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
    vec3 lightDir = normalize(u_lightPos - v_worldPosition);
    vec3 normal = normalize(v_worldNormal);
    
    vec4 flux = v_color * clamp(dot(normal, lightDir), 0.0, 1.0);
    float encodedFlux = encodeFlux(flux.xyz);
    
    vec3 decode = decodeFlux(encodedFlux);
    
    vec3 mainLightDirection = normal;
    
    if ( dot( mainLightDirection, lightDir ) < 0.0f )
    {
        normal *= -1.0f;
    }
    
    gl_FragData[0] = vec4(vec3(v_depth), 1);
    gl_FragData[1] = vec4(mainLightDirection, encodedFlux);
    gl_FragData[2] = vec4(v_worldPosition - normal * 0.2f, 1.0);
}