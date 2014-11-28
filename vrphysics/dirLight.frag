// Directional Light fragment shader
#version 120

// deferred g buffers
uniform sampler2DRect u_albedoTex;  // albedo (diffuse without lighting)
uniform sampler2DRect u_normalAndDepthTex;  // view space normal and linear depth
uniform sampler2DRect u_positionTex;

// LIGHTS
//uniform vec3 u_lightPosition;
uniform vec3 u_lightDir;
uniform vec4 u_lightAmbient;
uniform vec4 u_lightDiffuse;
uniform vec4 u_lightSpecular;
uniform float u_lightIntensity;

varying vec4 v_vertex;
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

const vec4 ambientGlobal = vec4(0.05, 0.05, 0.05, 1.0);

void main(void)
{
    vec2 texCoord = gl_FragCoord.xy ;
    
    vec3 vertex = texture2DRect(u_positionTex, texCoord.st).xyz;
    vec3 normal = texture2DRect(u_normalAndDepthTex, texCoord.st).xyz;
    
    vec4 ambient = vec4(0.0, 0.0, 0.0, 1.0);
    vec4 diffuse = vec4(0.0, 0.0, 0.0, 1.0);
    vec4 specular = vec4(0.0, 0.0, 0.0, 1.0);
    
//    vec3 lightDir = u_lightPosition;
    vec3 lightDir = u_lightDir;
    float lambert = max(dot(normal, normalize(lightDir)), 0.0);
    
    float linearDepth = texture2DRect(u_normalAndDepthTex, texCoord.st).a;
   
    // here check linear depth so that the background color won't be counted as a valid normal
    if (lambert > 0.0 && linearDepth != 1.0)
    {
        vec4 diffuseContribution = material1.diffuse * u_lightDiffuse * lambert;
        //diffuseContribution *= u_lightIntensity;
        
        vec3 R = normalize(reflect(lightDir, normal));
        vec3 V = normalize(vertex);
        
        vec4 specularContribution = material1.specular * u_lightSpecular * pow(max(dot(R, V), 0.0), material1.shininess);
        //specularContribution *= u_lightIntensity;
        
        diffuse += diffuseContribution;
        specular += specularContribution;
        gl_FragColor = vec4(0.0, 1.0, 0, 1);
    }
    else
    {
        gl_FragColor = vec4(1, 1, 1, 1);
    }
    
    vec4 final_color = vec4(ambient + diffuse + specular);
    gl_FragColor = vec4(final_color.rgb, 1.0);
//    gl_FragColor = vec4(linearDepth, linearDepth, linearDepth, 1);
}