#version 440 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D scene;
uniform sampler2D bloomBlur;
uniform float exposure;

void main()
{

    /*
    float stars = texture2D(scene, TexCoords).r;
    float glow = texture2D(bloomBlur, TexCoords).r * 0.15;
    vec3 color_1;
    vec3 color_2;


    color_1 = vec3(0.75, 0.25, 0.3);
    color_2 = vec3(0.3, 0.25, 0.3);
    FragColor = vec4(0.7 * mix(color_1, color_2 , 0.05 * (stars + glow)) * (stars + glow), 1.);

*/




    vec3 hdrColor = texture(scene, TexCoords).rgb;
    vec3 bloomColor = texture(bloomBlur, TexCoords).rgb * 0.6;

    vec3 result = hdrColor + bloomColor;
    result = vec3(1.0) - exp(-result * exposure);
    FragColor = vec4(result, 1.0);


    //FragColor = texture(bloomBlur, TexCoords);

}  