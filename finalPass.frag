// final pass fragment shader
#version 120

// deferred g buffers
uniform sampler2DRect u_albedoTex;  // albedo (diffuse without lighting)
uniform sampler2DRect u_dirLightPassTex;
uniform sampler2DRect u_pointLightPassTex;

varying vec2 v_texCoord;

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
    vec4 albedo = texture2DRect(u_albedoTex, v_texCoord.st);
   
    vec4 dirLightContribution = texture2DRect(u_dirLightPassTex, v_texCoord.st);
    vec4 pointLightContribution = texture2DRect(u_pointLightPassTex, v_texCoord.st);
    
    vec4 ambient = ambientGlobal + material1.ambient;
    
    vec4 lightContribution = vec4(0.0, 0.0, 0.0, 1.0);
    lightContribution += dirLightContribution;
    lightContribution += pointLightContribution;
    
    vec4 final_color = (ambient + lightContribution) * albedo;
    
    gl_FragColor = vec4(final_color.rgb, 1.0);
    //gl_FragColor = albedo;
    //gl_FragColor = vec4(1, 0, 1, 1);
}