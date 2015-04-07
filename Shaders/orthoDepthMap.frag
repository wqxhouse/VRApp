// orthoDepthMap.frag

#version 120

varying float v_depth;
varying vec4 v_color;

void main()
{
    // shadow part
    float moment1 = v_depth;
    float momentSq = v_depth * v_depth;
    
    float dx = dFdx(v_depth);
    float dy = dFdy(v_depth);
    momentSq += 0.25 * (dx*dx + dy*dy);
    
    gl_FragData[0] = vec4(moment1, momentSq, 0, 1);
}