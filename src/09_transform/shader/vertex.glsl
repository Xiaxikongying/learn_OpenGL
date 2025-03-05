#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec3 ourColor;
out vec2 TexCoord;

uniform float angleValue;
uniform mat4 transform;

mat4 rotate3d_z(float angle) {
    return mat4(
        cos(angle), -sin(angle), 0.0f, 0.0f,
        sin(angle),  cos(angle), 0.0f, 0.0f,
        0.0f,        0.0f,       1.0f, 0.0f,
        0.0f,        0.0f,       0.0f, 1.0f
    );
}

void main()
{
    //gl_Position = rotate3d_z(AngleValue) * vec4(aPos, 1.0f);
    gl_Position = transform * vec4(aPos, 1.0f);
    ourColor = aColor;
    TexCoord = aTexCoord;
}