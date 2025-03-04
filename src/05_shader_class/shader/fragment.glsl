#version 330 core
out vec4 FragColor;
in vec3 ourColor;
uniform vec3 colorOffset;

void main()
{
    FragColor = vec4((ourColor + colorOffset)/2, 1.0f);
}
