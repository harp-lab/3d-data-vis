#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

//fragment shader color depends on vertex
out vec3 ourColor;

void main()
{
    gl_Position = vec4(aPos, 1.0f);
    ourColor = aColor;
}
