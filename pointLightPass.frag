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
uniform float u_farDistance;

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
//  vec2 texCoord = gl_FragCoord.xy * u_inverseScreenSize.xy;
  
  float linearDepth = texture2DRect(u_normalAndDepthTex, texCoord.st).a;
  
  // vector to far plane
  //vec3 viewRay = vec3(v_vertex.xy * (-u_farDistance/v_vertex.z), -u_farDistance);
  //vec3 viewRay = vec3(vec2(v_vertex.x, v_vertex.z) * (u_farDistance/v_vertex.y), u_farDistance);
    
  // scale viewRay by linear depth to get view space position
  //vec3 vertex = viewRay * linearDepth;
    
    // ADDED: use position_map instead
    vec3 vertex = texture2DRect(u_positionTex, texCoord.st).xyz;
  
  vec3 normal = texture2DRect(u_normalAndDepthTex, texCoord.st).xyz;

  vec4 ambient = vec4(0.0, 0.0, 0.0, 1.0);
  vec4 diffuse = vec4(0.0, 0.0, 0.0, 1.0);
  vec4 specular = vec4(0.0, 0.0, 0.0, 1.0);

  vec3 lightDir = u_lightPosition - vertex;
  vec3 R = normalize(reflect(lightDir, normal));
  vec3 V = normalize(vertex);

  float lambert = max(dot(normal, normalize(lightDir)), 0.0);
    
    // I added later
   float realRadius = abs(length(v_vertex.xyz - u_lightPosition));
  
  if (lambert > 0.0) {
    float distance = length(lightDir);
    
    if (distance <= u_lightRadius) {
 //     if(distance <= realRadius) {
  
      float distancePercent = distance/u_lightRadius;
      float damping_factor = 1.0 - pow(distancePercent, 3);
      float attenuation = 1.0/(u_lightAttenuation.x +
                               u_lightAttenuation.y * distance +
                               u_lightAttenuation.z * distance * distance);
      attenuation *= damping_factor;
      
      vec4 diffuseContribution = material1.diffuse * u_lightDiffuse * lambert;
      //diffuseContribution *= u_lightIntensity;
      //diffuseContribution *= attenuation;
      
      vec4 specularContribution = material1.specular * u_lightSpecular * pow(max(dot(R, V), 0.0), material1.shininess);
      //specularContribution *= u_lightIntensity;
      //specularContribution *= attenuation;
      
      diffuse += diffuseContribution;
      specular += specularContribution;
        gl_FragColor = vec4(0.0, 1.0, 0, 1);
    }
    else
    {
        gl_FragColor = vec4(1, 0.0, 0.0, 1);
    }
  }
    else
    {
        gl_FragColor = vec4(1, 1, 1, 1);
    }
    //gl_FragColor = vec4(normal, 1);
    //gl_FragColor = vec4(1,  0, 1, 1);
    //gl_FragColor = u_lightDiffuse;
    //gl_FragColor = vec4(u_lightPosition, 1);
    
    //gl_FragColor = vec4(vertex, 1);
    //gl_FragColor = vec4(lightDir, 1);
    
  //vec4 final_color = vec4(ambient + diffuse + specular);
   // gl_FragColor = vec4(final_color.rgb, 1.0);
    //gl_FragColor = vec4(1, 1, 0, 1);
}
