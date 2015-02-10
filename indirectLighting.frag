#version 120

void main()
{
    vec4 result;
    
    // get surface info for this pixel
    vec4 wsPos     = tex2Dproj( DSSample0, fragment.pos2D );
    vec4 normal    = tex2Dproj( DSSample1, fragment.pos2D ) * 2.0f - 1.0f;
    
    // decode world space position (8 bit render target!)
    wsPos = ( wsPos - 0.5f ) * WSSIZE;
    
    // compute lighting
    float l2, lR, cosThetaI, cosThetaJ, Fij, phongExp;
    
    vec3 R  = fragment.lightPos - wsPos.xyz;						// R = vector from fragment to pixel light
    
    l2 = dot( R, R );												// squared length of R (needed again later)
    R *= rsqrt( l2 );												// normalize R
    lR = ( 1.0f / ( distBias + l2 * INV_WS_SIZE2_PI * 2 ) );	// distance attenuation (there's a global scene scaling factor "...WS_SIZE...")
    
    cosThetaI = saturate( dot( fragment.lightDir, -R ) );			// outgoing cosine
    
    phongExp = fragment.lightDir.w;
    if ( phongExp > 1.0f )
        cosThetaI = pow( cosThetaI, phongExp * l2 );				// with a phong like energy attenuation and widening of the high intensity region
    
    cosThetaJ = saturate( dot( normal,  R ) );						// incoming cosine
    Fij = cosThetaI * cosThetaJ * lR;								// putting everything together
    
#ifdef SMOOTH_FADEOUT
    vec3 t1 = fragment.center2D.xyz / fragment.center2D.w;
    vec3 t2 = fragment.pos2D.xyz    / fragment.pos2D.w;
    float fadeOutFactor = saturate( 2 - 6.667 * length( t1.xy - t2.xy ) / fragment.lightFlux.w );
    
    Fij *= fadeOutFactor;
#endif
    
    result = fragment.lightFlux * Fij;								// transfer energy!
    
    gl_FragColor = result;
}