// Directional Light fragment shader
#version 120

// deferred g buffers
uniform sampler2DRect u_albedoTex;  // albedo (diffuse without lighting)
uniform sampler2DRect u_normalAndDepthTex;  // view space normal and linear depth
uniform sampler2DRect u_positionTex;

uniform sampler2DRect u_depthMapTex;
uniform mat4 u_viewMatrixInverse;

uniform vec2 u_depthMapSize;
uniform float u_lightNearDistance;
uniform float u_lightFarDistance;

// LIGHTS
//uniform vec3 u_lightPosition;
uniform vec3 u_lightDir;
uniform vec4 u_lightAmbient;
uniform vec4 u_lightDiffuse;
uniform vec4 u_lightSpecular;
uniform float u_lightIntensity;

uniform mat4 u_lightViewMatrix;
uniform mat4 u_lightProjectionMatrix;

varying vec2 v_texCoord;
varying mat4 v_modelViewMatrix;

struct material {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float shininess;
};

const material material1 = material(
                                    vec4(0.1, 0.1, 0.1, 1.0),
                                    vec4(1.0, 1.0, 1.0, 1.0),
                                    vec4(1.0, 1.0, 1.0, 1.0),
                                    127.0
                                    );

const float SHADOW_EPSILON = 0.0002f;
float chebyshevUpperBound(vec2 moments, float myLinearZ)
{
    float variance = moments.y - (moments.x * moments.x);
    variance = max(variance, SHADOW_EPSILON);
    
    float d = myLinearZ - moments.x;
    float p_max = variance / (variance + d*d);
    return p_max;
}

float linstep(float low, float high, float v)
{
    return clamp((v-low)/(high-low), 0.0, 1.0);
}

float VSM(vec2 moments, float compare)
{
    float p = smoothstep(compare-0.00002, compare, moments.x);
    float variance = max(moments.y - moments.x*moments.x, -0.001);
    float d = compare - moments.x;
    float p_max = linstep(0.2, 1.0, variance / (variance + d*d));
    return clamp(max(p, p_max), 0.0, 1.0);
}

float sampleShadowMap(vec3 viewVertex)
{
    vec4 lightViewSpaceVec = u_lightViewMatrix * u_viewMatrixInverse * vec4(viewVertex, 1);
    vec4 lightProjSpaceVec = u_lightProjectionMatrix * lightViewSpaceVec;
    
    vec3 projCoords = lightProjSpaceVec.xyz / lightProjSpaceVec.w;
    vec2 uvCoords;
    uvCoords.x = 0.5 * projCoords.x + 0.5;
    uvCoords.y = 0.5 * projCoords.y + 0.5;
    
    vec2 rectCoords;
    
    rectCoords.x = uvCoords.x * u_depthMapSize.x;
    rectCoords.y = uvCoords.y * u_depthMapSize.y;
    //float sampleDepth = texture2DRect(u_depthMapTex, rectCoords).x;
    vec2 moments = texture2DRect(u_depthMapTex, rectCoords).xy;
    float linearZ = (-lightViewSpaceVec.z - u_lightNearDistance) / (u_lightFarDistance - u_lightNearDistance);
    float sampleDepth = moments.x;
    
//    if(linearZ <= sampleDepth + 0.01f)
//    {
//        return 1.0;
//    }
//    else
//    {
//        return chebyshevUpperBound(moments, linearZ) + 0.2f;
//        // return 0.1;
//    }
    return VSM(moments, linearZ);
    
//    if (sampleDepth + 0.005 < linearZ)
//    {
//        return 0.0;
//    }
//    else
//    {
//        return 1.0;
//    }
}

// https://www.unrealengine.com/blog/physically-based-shading-on-mobile
vec3 EnvBRDFApprox( vec3 SpecularColor, float Roughness, float NoV )
{
    const vec4 c0 = vec4( -1, -0.0275, -0.572, 0.022 );
    const vec4 c1 = vec4( 1, 0.0425, 1.04, -0.04 );
    vec4 r = Roughness * c0 + c1;
    float a004 = min( r.x * r.x, exp2( -9.28 * NoV ) ) * r.x + r.y;
    vec2 AB = vec2( -1.04, 1.04 ) * a004 + r.zw;
    return SpecularColor * AB.x + AB.y;
}

// GGX Normal distribution
float getNormalDistribution( float roughness4, float NoH )
{
    float d = ( NoH * roughness4 - NoH ) * NoH + 1;
    return roughness4 / ( d*d );
}

// Smith GGX geometric shadowing from "Physically-Based Shading at Disney"
float getGeometricShadowing( float roughness4, float NoV, float NoL, float VoH, vec3 L, vec3 V )
{
    float gSmithV = NoV + sqrt( NoV * (NoV - NoV * roughness4) + roughness4 );
    float gSmithL = NoL + sqrt( NoL * (NoL - NoL * roughness4) + roughness4 );
    return 1.0 / ( gSmithV * gSmithL );
}

// Fresnel term
vec3 getFresnel( vec3 specularColor, float VoH )
{
    vec3 specularColorSqrt = sqrt( clamp( vec3(0, 0, 0), vec3(0.99, 0.99, 0.99), specularColor ) );
    vec3 n = ( 1 + specularColorSqrt ) / ( 1 - specularColorSqrt );
    vec3 g = sqrt( n * n + VoH * VoH - 1 );
    return 0.5 * pow( (g - VoH) / (g + VoH), vec3(2.0) ) * ( 1 + pow( ((g+VoH)*VoH - 1) / ((g-VoH)*VoH + 1), vec3(2.0) ) );
}

void main(void)
{
    vec2 texCoord = gl_FragCoord.xy ;
    
    vec3 vertex = texture2DRect(u_positionTex, texCoord.st).xyz;
    vec4 normalSample = texture2DRect(u_normalAndDepthTex, texCoord.st);
    vec3 normal = normalSample.xyz;
    
    vec4 diffuse = vec4(0.0, 0.0, 0.0, 1.0);
    vec4 specular = vec4(0.0, 0.0, 0.0, 1.0);
    
    vec3 L = normalize(u_lightDir);
    float lambert = max(dot(normal, L), 0.0);
    
    float linearDepth = normalSample.a;
    
    // here check linear depth so that the background color won't be counted as a valid normal
    //if (lambert > 0.0 && linearDepth != 1.0)
    if (lambert > 0.0)
    {
//        vec4 diffuseContribution = material1.diffuse * u_lightDiffuse * lambert;
//        vec4 diffuseContribution = u_lightDiffuse * lambert;
        float metallic = 0.3f;
        
        vec4 baseColor = vec4(1, 1, 1, 1);
        vec4 diffuseContribution = lambert * (baseColor - baseColor * metallic);
        // vec3 diffuse = vec3(1, 1, 1) - vec3(1, 1, 1) * metallic;
        
//        vec3 R = normalize(reflect(lightDir, normal));
//        vec3 V = normalize(vertex);
        // vec4 specularContribution = material1.specular * u_lightSpecular * pow(max(dot(R, V), 0.0), material1.shininess);
        
        vec3 specularColor = mix( vec3( 0.08 * u_lightSpecular ), vec3(1, 1, 1), metallic);
        vec3 V = normalize(-vertex);
        vec3 H = normalize(V + L);
        
        // get all the useful dot products and clamp them between 0 and 1 just to be safe
        float NoV			= max( dot( normal, L ), 0.0 );
        float VoH			= max( dot( V, H ), 0.0 );
        float NoH			= max( dot( normal, H ), 0.0 );
        
        float roughness = 0.6;
        roughness = pow(roughness, 4);
        
        // compute the brdf terms
        float distribution	= getNormalDistribution( roughness, NoH );
        vec3 fresnel		= getFresnel( specularColor, VoH );
        float geom			= getGeometricShadowing( roughness, NoV, lambert, VoH, L, V );
        
        // get the specular and diffuse and combine them
        specular		= vec4( lambert * ( distribution * fresnel * geom ), 1.0);
        diffuse += diffuseContribution;
        
        // specular += specularContribution;
    }
    
    float depth = sampleShadowMap(vertex);
    vec4 final_color = depth * u_lightDiffuse * (diffuse + specular);
    gl_FragColor = vec4(final_color.rgb, 1.0);
}