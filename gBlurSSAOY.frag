#version 120

uniform sampler2DRect u_ssaoTex;
uniform sampler2D u_depthTex;
uniform vec2 u_inv_screenSize;
uniform float fs;
uniform float fd;

float Offsets[8] = float[](-4.0, -3.0, -2.0, -1.0, 1.0, 2.0, 3.0, 4.0);
float BlurWeights[8] = float[](1.0, 2.0, 3.0, 4.0, 4.0, 3.0, 2.0, 1.0);

void main()
{
    float BlurWeightsSum = 5.0;
    
    vec2 texcoord = gl_FragCoord.xy * u_inv_screenSize;
    float SSAO = texture2DRect(u_ssaoTex, gl_FragCoord.xy).r * BlurWeightsSum;
    float Depth = texture2D(u_depthTex, texcoord).r;
    
    float Factor = fs - fd * Depth;
    
    for(int i = 0; i < 8; i++)
    {
        vec2 depthTexCoord = vec2(texcoord.x,  Offsets[i] * u_inv_screenSize.y + texcoord.y);
        vec2 ssaoTexCoord = vec2(gl_FragCoord.x, Offsets[i] + gl_FragCoord.y);
        
        float depth = texture2D(u_depthTex, depthTexCoord).r;
        
        if(abs(Depth - depth) < Factor)
        {
            SSAO += texture2DRect(u_ssaoTex, ssaoTexCoord).r * BlurWeights[i];
            BlurWeightsSum += BlurWeights[i];
        }
    }
    
    gl_FragColor = vec4(vec3(SSAO / BlurWeightsSum), 1.0);
    //gl_FragColor = vec4(0, 1, 0, 1);
}