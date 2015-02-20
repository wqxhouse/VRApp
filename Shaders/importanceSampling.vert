// importanceSampling.vert

#version 120
uniform sampler2D u_poissowTex;

void main()
{
// debug poisson texture
//    vec2 texcoord = gl_Vertex.xy;
//    vec4 pos = texture2D(u_poissowTex, texcoord);
//    pos = pos * 2.0 - 1.0;
//    pos.zw = vec2(0, 1);
//    
//    gl_Position = pos;
    
   gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}