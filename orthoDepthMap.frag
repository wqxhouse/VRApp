#version 120

varying float v_depth;
varying vec3 v_color;
void main()
{
//   gl_FragData[0] = v_depth;
    //gl_FragColor = vec4(1, 0, 0, 1);
    //gl_FragData[0] = vec4(1, 0, 1, 1);
   // gl_FragData[0] = vec4(v_color, 1);
    gl_FragData[0] = vec4(1, 0, 0, 1);
    //gl_FragColor = vec4(1, 0, 1, 1);
}