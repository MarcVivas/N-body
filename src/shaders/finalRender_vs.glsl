#version 440 core

layout (location = 0) in vec3 texturePos;
layout (location = 1) in vec2 textureCoords;

out vec2 TexCoords;

void main()
{
    gl_Position = vec4(texturePos.x, texturePos.y, 0.0, 1.0);
    TexCoords = textureCoords;
}