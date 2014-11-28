// Geometry pass fragment shader
#version 120

uniform sampler2DRect u_texture;

varying vec3 v_normal;
varying vec2 v_texCoord;
varying float v_depth;

varying vec4 v_color;
varying vec4 v_position;

void main(void)
{
  //vec3 diffuse = texture2DRect(u_texture, v_texCoord.st).rgb;
  //gl_FragData[0] = vec4(diffuse, 1.0); // albedo
    gl_FragData[0] = v_color;
    gl_FragData[1] = vec4(normalize(v_normal), v_depth); // normals + depth
    gl_FragData[2] = vec4(v_position.xyz, 0);
   // gl_FragColor = vec4(1, 0, 1, 1);
    //gl_FragData[0] = vec4(1, 1 ,1, 1);
    //gl_FragData[1] = vec4(1, 0, 0, 1);
}