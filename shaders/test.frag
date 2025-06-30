#version 330 core
out vec4 FragColor;
in vec4 givenColor;
void main()
{
   FragColor = givenColor;
}