#version 440 core
out vec4 FragColor;
in vec3 vel;
void main()
{
    FragColor = vec4(vel, 1.0f);
}