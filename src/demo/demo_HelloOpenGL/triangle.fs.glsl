#version 460 core
in vec3 TexPosition;
in vec2 TexCoords;
out vec4 FragColor;
void main()
{
    FragColor=vec4(TexPosition, 1);
}