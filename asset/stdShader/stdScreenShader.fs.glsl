#version 460 core
in vec2 TexCoords;
out vec4 FragColor;
uniform sampler2D screenTexture;
void main()
{
    // float tmp=pow(texture(screenTexture,TexCoords).r,64);
    FragColor=vec4(texture(screenTexture,TexCoords).rgb,1);
    // FragColor=vec4(vec3(tmp),1);
}