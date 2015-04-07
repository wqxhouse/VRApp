// toneMapping frag shader
#version 120
uniform sampler2DRect u_screenTex;
float A = 0.15;
float B = 0.50;
float C = 0.10;
float D = 0.20;
float E = 0.02;
float F = 0.30;
float W = 11.2;

//vec3 RGB_TO_LUM = vec3(0.2126f, 0.7152f, 0.0722f);

//float EyeAdaption(float lum)
//{
//    return lerp(0.2f, lum, 0.5f);
//}

vec3 Uncharted2Tonemap(vec3 x)
{
    return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}

void main()
{
    vec3 inTex = texture2DRect(u_screenTex, gl_FragCoord.xy).rgb;
//    inTex *= 16;
//    float lum = dot(color, RGB_TO_LUM);
//    float adapted_lum_dest = 3 / (max(0.1f, 1 + 10 * EyeAdaption(adapted_lum)));
    
//    // apply the tone-mapping
//    color					= Uncharted2Tonemap( color * uExposure );
//    
//    // white balance
//    const float whiteInputLevel = 20.0f;
//    vec3 whiteScale			= 1.0f / Uncharted2Tonemap( vec3( whiteInputLevel ) );
//    color					= color * whiteScale;
//    
//    // gamma correction
//    color					= pow( color, vec3( 1.0f / uGamma ) );
    
//    float ExposureBias = 0.5;
    float ExposureBias = 1.0;
    vec3 curr = Uncharted2Tonemap(ExposureBias * inTex);
    
    vec3 whiteScale = 1.0 / Uncharted2Tonemap(vec3(W));
    vec3 color = curr * whiteScale;
    
    float expFactor = 1 / 2.2;
    vec3 retColor = pow(color, vec3(expFactor));
    
    gl_FragColor = vec4(retColor, 1);
}