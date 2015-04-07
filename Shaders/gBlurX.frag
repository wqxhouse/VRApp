// gaussian blur X fragment shader
#version 120
uniform sampler2DRect u_texture;
// uniform vec2 resolution;

// const float blurSize = 3/512.0;
const float blurSize = 0.5;

void main(void)
{
    vec4 sum = vec4(0.0);
    // vec2 vTexCoord = gl_FragCoord.xy / resolution.xy;
    vec2 vTexCoord = gl_FragCoord.xy;
    
    // blur in y (vertical)
    // take nine samples, with the distance blurSize between them
    sum += texture2DRect(u_texture, vec2(vTexCoord.x, vTexCoord.y - 4.0*blurSize)) * 0.05;
    sum += texture2DRect(u_texture, vec2(vTexCoord.x, vTexCoord.y - 3.0*blurSize)) * 0.09;
    sum += texture2DRect(u_texture, vec2(vTexCoord.x, vTexCoord.y - 2.0*blurSize)) * 0.12;
    sum += texture2DRect(u_texture, vec2(vTexCoord.x, vTexCoord.y - blurSize)) * 0.15;
    sum += texture2DRect(u_texture, vec2(vTexCoord.x, vTexCoord.y)) * 0.16;
    sum += texture2DRect(u_texture, vec2(vTexCoord.x, vTexCoord.y + blurSize)) * 0.15;
    sum += texture2DRect(u_texture, vec2(vTexCoord.x, vTexCoord.y + 2.0*blurSize)) * 0.12;
    sum += texture2DRect(u_texture, vec2(vTexCoord.x, vTexCoord.y + 3.0*blurSize)) * 0.09;
    sum += texture2DRect(u_texture, vec2(vTexCoord.x, vTexCoord.y + 4.0*blurSize)) * 0.05;
    
    gl_FragColor = sum;
//    vec4 s = texture2DRect(u_texture, gl_FragCoord.xy);
//    gl_FragColor = s;
    // gl_FragColor = vec4(0, 1, 0, 1);
}