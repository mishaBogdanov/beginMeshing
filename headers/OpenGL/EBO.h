#pragma once
#include <glad/glad.h>
#include <vector>

class EBO
{
	public:
		GLuint ID;
		EBO(std::vector<GLuint>& given);
		EBO(GLuint* indicies, GLsizeiptr size);
		EBO();

		void Bind();
		void Unbind();
		void Delete();
};

