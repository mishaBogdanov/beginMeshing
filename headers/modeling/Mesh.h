#pragma once
#include "Body.h"
#include "glm/glm.hpp"
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <string>
class Mesh
{
public:
	Mesh(std::string inName, float inScale = 1, glm::vec3 inLocation = {0.0, 0.0, 0.0});
	Mesh(std::vector<std::vector<glm::vec3>> inLocations, std::vector<std::vector<GLuint>> indices);
	const glm::mat4& GetMat() const { return mCurPosMatrix; }
	void Draw(ShaderClass& inShader);
private:
	void Load(std::string inName, float inScale = 1, glm::vec3 inLocation = { 0.0, 0.0, 0.0 });
private:
	std::unordered_set <std::unique_ptr<Body>> mBodies;
	glm::vec3                 mOriginalCm;
	glm::vec3                 mCurCm;
	glm::mat4                 mCurPosMatrix;
	int64_t                   id;
	std::vector<glm::vec3> mBoundingBox;
};

