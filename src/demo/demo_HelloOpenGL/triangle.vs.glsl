#version 460 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoords;

out vec3 TexPosition;
out vec2 TexCoords;

void main()
{
    TexPosition = position;
    TexCoords = texCoords;
    gl_Position = vec4(position, 1);
}
