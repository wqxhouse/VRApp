// Point light fragment shader
#version 120

// deferred g buffers
uniform sampler2DRect u_albedoTex;  // albedo (diffuse without lighting)
uniform sampler2DRect u_normalAndDepthTex;  // view space normal and linear depth
uniform sampler2DRect u_positionTex;

// LIGHTS
uniform int u_numLights;
uniform vec3 u_lightPosition;
uniform vec4 u_lightAmbient;
uniform vec4 u_lightDiffuse;
uniform vec4 u_lightSpecular;
uniform vec3 u_lightAttenuation;
uniform float u_lightIntensity;
uniform float u_lightRadius;

uniform vec2 u_inverseScreenSize;

varying vec4 v_vertex;
varying vec2 v_texCoord;
varying mat4 v_modelViewMatrix;

varying vec4 v_pos;

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

// TODO: fix when camera in light sphere, light disappear

void main(void)
{
    vec2 texCoord = gl_FragCoord.xy ;
    vec3 vertex = texture2DRect(u_positionTex, texCoord.st).xyz;
    vec3 normal = texture2DRect(u_normalAndDepthTex, texCoord.st).xyz;
    
    vec4 ambient = vec4(0.0, 0.0, 0.0, 1.0);
    vec4 diffuse = vec4(0.0, 0.0, 0.0, 1.0);
    vec4 specular = vec4(0.0, 0.0, 0.0, 1.0);
    
    vec3 lightDir = u_lightPosition - vertex;
    vec3 R = normalize(reflect(lightDir, normal));
    vec3 V = normalize(vertex);
    
    float lambert = max(dot(normal, normalize(lightDir)), 0.0);
    
    if (lambert > 0.0) // TODO: think if this can really happen (back face ?)
    {
        float distance = length(lightDir);
        // distance checking isn't necessary since stencil test handles that
        //if (distance <= u_lightRadius)
        {
            //float distancePercent = distance/u_lightRadius;
            //float damping_factor = 1.0 - pow(distancePercent, 3);
            float attenuation = u_lightAttenuation.x +
                                u_lightAttenuation.y * distance +
                                u_lightAttenuation.z * distance * distance;
            //attenuation *= damping_factor;
            attenuation = max(1.0, attenuation);
            
            vec4 diffuseContribution = material1.diffuse * u_lightDiffuse * lambert;
            diffuseContribution *= u_lightIntensity;
            diffuseContribution /= attenuation;
            
            vec4 specularContribution = material1.specular * u_lightSpecular * pow(max(dot(R, V), 0.0), material1.shininess);
            specularContribution *= u_lightIntensity;
            specularContribution /= attenuation;
            
            diffuse += diffuseContribution;
            specular += specularContribution;
        }
    }
    
    vec4 final_color = vec4(ambient + diffuse + specular);
    gl_FragColor = vec4(final_color.rgb, 1.0);
}

