#version 120

varying vec3 TexCoord0;
uniform samplerCube u_CubemapTexture;

void main()
{
    gl_FragColor = textureCube(u_CubemapTexture, TexCoord0);
    // gl_FragColor = vec4(1, 1, 1, 1);
}