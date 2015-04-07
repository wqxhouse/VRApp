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

const float distBias = 1.0f; // avoid singularity

varying float v_radius;

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
    vec2 rectCoord = gl_FragCoord.xy;
    
    // do not include w component, or else inverse matrix will be incorrectly multiplied.
    vec3 viewSpacePosition = texture2DRect(u_viewPositionTex, rectCoord).xyz;
    vec4 worldSpacePosition = u_viewInverseMatrix * vec4(viewSpacePosition, 1);
    
    vec3 lightDir  = v_lightPos.xyz - worldSpacePosition.xyz;
    float fragToLightRadius = length(lightDir);
    
    // dynamic branching -- not sure how this impacts performance
    // use to limit pixels between the eye and the backface of light sphere
    if( fragToLightRadius > v_radius ) // TODO: need to make sure v_radius is the intended radius
    {
        discard;
//        gl_FragColor = vec4(0, 1, 0, 1);
//        return;
    }
    
    vec3 viewSpaceNormal = texture2DRect(u_viewNormalTex, rectCoord).xyz;
    vec3 worldSpaceNormal   = (u_viewInverseMatrix * vec4(viewSpaceNormal, 0)).xyz;

//    vec3 L = normalize(lightDir);
//    float ndotL = dot(worldSpaceNormal, L);
//    
//    vec4 result = vec4(0, 0, 0, 1);
//    
//    if(ndotL > 0.0)
//    {
//        float distSqr = dot(lightDir, lightDir);
//        float atten = 1.0 / (1.0 + distSqr);
//        vec3 diffuse = v_lightFlux.rgb * vec3(ndotL);
//        
//        result.rgb = diffuse * atten;
//    }
//    
    
    vec3 R = lightDir;
    
//    // compute lighting
    float l2, lR, cosThetaI, cosThetaJ, Fij, phongExp;
    
    // squared length of R (needed again later)
    l2 = dot( R, R );
    
    // normalize R
    R *= inversesqrt( l2 );
    
    // distance attenuation,
    // lR = ( 1.0f / ( distBias + l2 * 0.03 * 6.28) );
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
    
//    //     fade out
//    vec2 t1 = ( v_center2D.xy / v_center2D.w ) * 0.5 + 0.5;
//        vec2 t2 = gl_FragCoord.xy * u_render_wh_inv;
//        float fadeOutFactor = clamp( 2 - 6.667 * length( t1 - t2 ) / v_lightFlux.w, 0.0, 1.0 );
//    
//        Fij *= fadeOutFactor;

    vec4 result = v_lightFlux * Fij;								// transfer energy!
    
    gl_FragColor = result;
}


void visualizeVPLPos()
{
//    gl_FragColor = vec4(0.2, 0.2, 0.2, 0.2);
//    gl_FragColor = vec4(0, 1, 0, 1);
    
    vec2 rectCoord = gl_FragCoord.xy;
    
    // do not include w component, or else inverse matrix will be incorrectly multiplied.
    vec3 viewSpacePosition = texture2DRect(u_viewPositionTex, rectCoord).xyz;
    vec4 worldSpacePosition = u_viewInverseMatrix * vec4(viewSpacePosition, 1);
    
    // R = vector from fragment to pixel light
    vec3 R  = v_lightPos.xyz - worldSpacePosition.xyz;
    float fragToLightRadius = length(R);
    
    // dynamic branching -- not sure how this impacts performance
    if( fragToLightRadius > v_radius )
    {
        // discard;
        gl_FragColor = vec4(0, 1, 0, 1);
    }
    else
    {
        gl_FragColor = vec4(1, 0, 0, 1);
    }
}

void visualizeVPLFlux()
{
    gl_FragColor = v_lightFlux;
//    gl_FragColor = vec4(fluxDebug, 1);
}

void main()
{
     impl1();
    // visualizeVPLPos();
    // visualizeVPLFlux();
}
