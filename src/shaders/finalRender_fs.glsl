#version 440 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D normalScene;
uniform sampler2D blurScene;
uniform float intensity;

void main()
{
    vec3 normalColor = texture(normalScene, TexCoords).rgb;
    vec3 blurColor = texture(blurScene, TexCoords).rgb;

    vec3 result = normalColor + blurColor * intensity;
    FragColor = vec4(result, 1.0);


    //FragColor = texture(bloomBlur, TexCoords);

}