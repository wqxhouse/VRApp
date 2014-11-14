#version 120

uniform sampler2DRect tex;
varying vec2 texcoord;
void main()
{
    vec3 diffuse = texture2DRect(tex, texcoord).xyz;
    //gl_FragColor = vec4(1, 0, 1, 1);
    gl_FragColor = vec4(diffuse, 1);
}