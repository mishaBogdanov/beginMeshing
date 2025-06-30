#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 positionMatrix;

void main()
{
   gl_Position = positionMatrix * vec4(aPos.x, aPos.y, aPos.z, 1.0);
}