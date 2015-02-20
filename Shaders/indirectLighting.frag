#version 120
uniform sampler2DRect u_sampleTexcoordTex;

uniform sampler2D u_samplePosTex;
uniform vec2 u_render_wh;
uniform vec2 u_render_wh_inv;

uniform sampler2DRect u_worldPosTex;
uniform sampler2DRect u_lightDirTex;

varying vec3 v_samplePos;
varying vec4 v_lightPos;
varying vec4 v_lightDir;

varying vec4 v_lightFlux;
// varying vec4 v_texcoord;
varying vec4 v_center2D;

uniform mat4 u_viewInverseMatrix;

uniform sampler2DRect u_viewPositionTex;
uniform sampler2DRect u_viewNormalTex;

uniform vec3 u_mainLightWorldPos;

varying vec3 fluxDebug;

const float distBias = 0.01f; // avoid singularity

void try()
{
    vec2 rectCoord = gl_FragCoord.xy;
    vec3 viewSpacePosition = texture2DRect(u_viewPositionTex, rectCoord).xyz;
    vec3 viewSpaceNormal = texture2DRect(u_viewNormalTex, rectCoord).xyz;
    vec3 worldSpacePosition = vec3(u_viewInverseMatrix * vec4(viewSpacePosition, 1)); //
    vec3 worldSpaceNormal   = normalize(vec3(u_viewInverseMatrix * vec4(viewSpaceNormal, 0)));
    
    vec3 toVPL = v_lightPos.xyz - worldSpacePosition;
    float distToVPLSqr = max( dot( toVPL, toVPL ), 0.2 );
    
    toVPL = normalize( toVPL );
    
    float len_Lj = distance(u_mainLightWorldPos, v_lightPos.xyz);
    float len_Lj_sqr = len_Lj * len_Lj;
    
    vec3 norm_i = worldSpaceNormal;
    vec3 norm_j = normalize(v_lightDir.xyz);
    
    float NiDotT = max( 0.0, dot(norm_i, toVPL) );
    float NjDotT = max( 0.0, dot(norm_j, -toVPL) );
    
    float numer = len_Lj_sqr * NjDotT * NjDotT;
    float denom = 1.5 * 4096 * distToVPLSqr + len_Lj_sqr;
    
    vec3 finalColor = ((20 * numer) / denom) * v_lightFlux.xyz;
    gl_FragColor = vec4(finalColor, 1);
    
}

void debug()
{
    //    int SIZE = 64;
    //    vec4 color = vec4(0, 0, 0, 0);
    //    if(gl_FragCoord.x < SIZE && gl_FragCoord.y < SIZE)
    //    {
    //        // Here we need floor the gl_FragCoord.xy since gl_FragCoord is (0.5, 0.5) instead of (0.0, 0.0)
    //        int index = int(gl_FragCoord.y) * SIZE + int(gl_FragCoord.x);
    //        vec4 cc = texture2DRect(u_sampleTexcoordTex, vec2(index, 0));
    //        color = texture2D(u_samplePosTex, cc.xy);
    //    }
    //    else
    //    {
    //        discard;
    //    }
    //     gl_FragColor = vec4(color.xy, 0, 1);
}

void impl1()
{
    vec4 result;
    
    // get surface info for this pixel
    vec2 rectCoord = gl_FragCoord.xy;
    
    // do not include w component, or else inverse matrix will be incorrectly multiplied.
    vec3 viewSpacePosition = texture2DRect(u_viewPositionTex, rectCoord).xyz;
    //    vec4 viewSpaceNormal = texture2DRect(u_viewNormalTex, rectCoord) * 2.0 - 1.0;
    vec3 viewSpaceNormal = texture2DRect(u_viewNormalTex, rectCoord).xyz;
    
    vec4 worldSpacePosition = u_viewInverseMatrix * vec4(viewSpacePosition, 1);
    vec4 worldSpaceNormal   = u_viewInverseMatrix * vec4(viewSpaceNormal, 0);
    
    // compute lighting
    float l2, lR, cosThetaI, cosThetaJ, Fij, phongExp;
    
    // R = vector from fragment to pixel light
    vec3 R  = v_lightPos.xyz - worldSpacePosition.xyz;
    
    // squared length of R (needed again later)
    l2 = dot( R, R );
    
    // normalize R
    R *= inversesqrt( l2 );
    
    // distance attenuation (there's a global scene scaling factor "...WS_SIZE...")
    //    lR = ( 1.0f / ( distBias + l2 * INV_WS_SIZE2_PI * 2 ) );
    lR = ( 1.0f / ( distBias + l2 ) );
    
    cosThetaI = clamp( dot( v_lightDir.xyz, -R ), 0.0, 1.0);			// outgoing cosine
    
    phongExp = v_lightDir.w;
    
    if ( phongExp > 1.0f )
    {
        // with a phong like energy attenuation and widening of the high intensity region
        cosThetaI = pow( cosThetaI, phongExp * l2 );
    }
    
    // incoming cosine
    cosThetaJ = clamp( dot( worldSpaceNormal.xyz,  R ), 0.0, 1.0 );
    
    // putting everything together
    Fij = cosThetaI * cosThetaJ * lR;
    
    //     fade out
//    vec2 t1 = v_center2D.xy * 0.5 + 0.5;;
//        vec2 t2 = gl_FragCoord.xy * u_render_wh_inv;
//        float fadeOutFactor = clamp( 2 - 6.667 * length( t1 - t2 ) / v_lightFlux.w, 0.0, 1.0 );
//    
//        Fij *= fadeOutFactor;
    
    result = v_lightFlux * Fij;								// transfer energy!
    gl_FragColor = result;
}

void main()
{
    impl1();
}
