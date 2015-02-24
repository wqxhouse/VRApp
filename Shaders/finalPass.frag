// final pass fragment shader
#version 120

// deferred g buffers
uniform sampler2DRect u_albedoTex;  // albedo (diffuse without lighting)
uniform sampler2DRect u_dirLightPassTex;
uniform sampler2DRect u_ssaoPassTex;
uniform sampler2DRect u_pointLightPassTex;
uniform sampler2DRect u_indLightPassTex;

varying vec2 v_texCoord;

struct material {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float shininess;
};

//const material material1 = material(
//                                    vec4(0.2, 0.2, 0.2, 1.0),
//                                    vec4(1.0, 1.0, 1.0, 1.0),
//                                    vec4(1.0, 1.0, 1.0, 1.0),
//                                    127.0
//                                    );

const vec4 ambientGlobal = vec4(0.05, 0.05, 0.05, 1.0);
//const vec4 ambientGlobal = vec4(1.0, 1.0, 1.0, 1.0);

void main(void)
{
    vec4 albedo = texture2DRect(u_albedoTex, v_texCoord.st);
   
    vec4 dirLightContribution = texture2DRect(u_dirLightPassTex, v_texCoord.st);
    vec4 pointLightContribution = texture2DRect(u_pointLightPassTex, v_texCoord.st);
    float ssaoContribution = texture2DRect(u_ssaoPassTex, v_texCoord.st).r;
    
    vec4 indirectLightContribution = texture2DRect(u_indLightPassTex, v_texCoord.st);
    
    // vec4 ambient = ambientGlobal + material1.ambient;
    vec4 ambient = vec4(0, 0, 0, 1);
    
    vec4 lightContribution = vec4(0.0, 0.0, 0.0, 1.0);
    lightContribution += dirLightContribution;
    lightContribution += pointLightContribution;
    lightContribution += indirectLightContribution;
    
//    vec4 final_color = (ambient * ssaoContribution + lightContribution) * albedo;
    vec4 final_color = ( ambient + vec4(lightContribution.rgb, 1) ) * albedo * ssaoContribution;
//    vec4 final_color = ( ambient + vec4(lightContribution.rgb, 1) ) *  ssaoContribution;
    
    gl_FragColor = vec4(final_color.rgb, 1.0);
}