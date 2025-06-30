#version 330 core


layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;



out vec4 givenColor;

void main()
{
    for (int i = 0; i < 3; ++i)
    {
        gl_Position = gl_in[i].gl_Position; 
        givenColor = vec4(1.0, 1.0, 0.0, 1.0);      
        EmitVertex();
    }
    EndPrimitive();
}