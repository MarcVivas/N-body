#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D image;
uniform bool horizontal;

const int kernel_size = 13;
uniform float weight[kernel_size] = float[] (
    0.090572, 0.098889, 0.103768, 0.10591, 
    0.10512, 0.10137, 0.094814, 0.0857, 
    0.074365, 0.061311, 0.047085, 0.032248, 
    0.01734
);

void main()
{
    vec2 tex_offset = 1.0 / textureSize(image, 0); // Size of one texel
    vec2 direction = horizontal ? vec2(tex_offset.x, 0.0) : vec2(0.0, tex_offset.y);

    vec3 result = texture(image, TexCoords).rgb * weight[0];

    for(int i = 1; i < kernel_size; ++i)
    {
        vec2 offset = direction * i;
        result += texture(image, TexCoords + offset).rgb * weight[i];
        result += texture(image, TexCoords - offset).rgb * weight[i];
    }

    FragColor = vec4(result, 1.0);
}