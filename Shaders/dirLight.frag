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

const float SHADOW_EPSILON = 0.0005f;
float chebyshevUpperBound(vec2 moments, float myLinearZ)
{
    float variance = moments.y - (moments.x * moments.x);
    variance = max(variance, 0.00002);
    
    float d = myLinearZ - moments.x;
    float p_max = variance / (variance + d*d);
    return p_max;
    
//    float E_x2 = moments.y;
//    float Ex_2 = moments.x * moments.x;
//    float variance = min(max(E_x2 - Ex_2, 0.0) + SHADOW_EPSILON, 1.0);
//    float m_d = (moments.x - myLinearZ);
//    float p = variance / (variance + m_d * m_d); //Chebychev's inequality
//    return p;
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
    
    if(linearZ <= sampleDepth )
    {
        return 1.0;
    }
    else
    {
        return chebyshevUpperBound(moments, linearZ) + 0.2f;
    }
    
//    if (sampleDepth + 0.005 < linearZ)
//    {
//        return 0.0;
//    }
//    else
//    {
//        return 1.0;
//    }
}

void main(void)
{
    vec2 texCoord = gl_FragCoord.xy ;
    
    vec3 vertex = texture2DRect(u_positionTex, texCoord.st).xyz;
    vec4 normalSample = texture2DRect(u_normalAndDepthTex, texCoord.st);
    vec3 normal = normalSample.xyz;
    
//    vec4 ambient = vec4(0.0, 0.0, 0.0, 1.0);
    vec4 diffuse = vec4(0.0, 0.0, 0.0, 1.0);
    vec4 specular = vec4(0.0, 0.0, 0.0, 1.0);
    
    vec3 lightDir = u_lightDir;
    float lambert = max(dot(normal, normalize(lightDir)), 0.0);
    
    float linearDepth = normalSample.a;
    
    // here check linear depth so that the background color won't be counted as a valid normal
    if (lambert > 0.0 && linearDepth != 1.0)
    {
        vec4 diffuseContribution = material1.diffuse * u_lightDiffuse * lambert;
        
        vec3 R = normalize(reflect(lightDir, normal));
        vec3 V = normalize(vertex);
        
        vec4 specularContribution = material1.specular * u_lightSpecular * pow(max(dot(R, V), 0.0), material1.shininess);
        
        diffuse += diffuseContribution;
        specular += specularContribution;
    }
    
    float depth = sampleShadowMap(vertex);
    vec4 final_color = depth * (diffuse + specular);
    gl_FragColor = vec4(final_color.rgb, 1.0);
}