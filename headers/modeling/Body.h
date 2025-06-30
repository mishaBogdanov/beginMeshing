#pragma once
#include "Point.h"
#include "Triangle.h"
#include "../OpenGL/VAO.h"
#include <unordered_map>
#include "../OpenGL/ShaderClass.h"
class Mesh;
class Body
{
public:
	Body(std::vector <glm::vec3>& inVertices, std::vector <GLuint>& inIndices, Mesh* inParent);
	void Draw(ShaderClass& inShader);
private:
	VAO                                  mVAO;
	glm::mat4                            mPosition;
	Mesh*                                mParentMesh; 
	std::vector<glm::vec3>               mPoints;
	std::vector<GLuint>                  mIndicies;
	std::vector<Point>                   mPointData;
	std::unordered_map<int64_t, int64_t> mPointId2Idx;  // point id 2 idx in mPointData
	std::vector<Triangle>                mTriangleData;
	std::unordered_map<int64_t, int64_t> mTriId2Idx;    // tri id 2 idx in mTriangleData
};

