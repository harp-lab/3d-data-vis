#version 330 core
out vec4 FragColor;

// The same name of out in vertex shader source
in vec3 ourColor;

void main()
{
    FragColor = vec4(ourColor, 1.0f);
}
