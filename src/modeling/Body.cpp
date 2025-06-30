#include "../../headers/modeling/Body.h"
#include "../../headers/OpenGL/EBO.h"
#include "../../headers/modeling/Mesh.h"

Body::Body(std::vector <glm::vec3>& gVertices, std::vector <GLuint>& gIndices, Mesh* inParent)
{
	mPoints   = gVertices;
	mIndicies = gIndices;
	mVAO.Bind();
	VBO VBO(mPoints);
	EBO EBO(mIndicies);
	mVAO.LinkAtrib(VBO, 0, 3, GL_FLOAT, sizeof(glm::vec3), (void*)0);
	mPosition = glm::mat4(1.0f);
	mVAO.Unbind();
	VBO.Unbind();
	EBO.Unbind();
	mParentMesh = inParent;
}
void
Body::Draw(ShaderClass& inShader)
{
	auto finalMat = mPosition * mParentMesh->GetMat();
	glUniformMatrix4fv(glGetUniformLocation(inShader.ID, "positionMatrix"), 1, GL_FALSE, glm::value_ptr(finalMat));
	mVAO.Bind();
	glDrawElements(GL_TRIANGLES, mIndicies.size(), GL_UNSIGNED_INT, 0);
}
