#include "../../headers/modeling/Mesh.h"
#include <iostream>
#include <fstream>

Mesh::Mesh(std::string inName, float scale, glm::vec3 inLocation)
{
	mCurPosMatrix = glm::mat4(1);
	if (!inName.empty())
	{
		Load(inName, scale, inLocation);
	}
}
Mesh::Mesh(std::vector<std::vector<glm::vec3>> inLocations, std::vector<std::vector<GLuint>> indices)
{
	mCurPosMatrix = glm::mat4(1);
	for (size_t i = 0; i < inLocations.size(); i++)
	{
		std::vector<glm::vec3> Vertices = std::move(inLocations[i]);
		std::vector<GLuint> Indices = std::move(indices[i]);
		mBodies.insert(std::make_unique<Body>(Vertices, Indices, this));
	}
}
void
Mesh::Draw(ShaderClass& inShader)
{
	for (auto& body : mBodies)
	{
		body->Draw(inShader);
	}
}
void 
Mesh::Load(std::string inName, float inScale, glm::vec3 inLocation)
{
	std::ifstream file(inName);
	std::vector<glm::vec3> Vertices = {};
	std::vector<GLuint> Indices = {};
	int currentVertexesNumber = 0;
	int lastMaxVertexNumber = 1;

	float PosX, PosY, PosZ, NegX, NegY, NegZ;

	bool isFirst = true;

	if (file.is_open()) {
		std::istream_iterator<std::string> fileIterator(file);
		std::istream_iterator<std::string> endIterator;

		while (fileIterator != endIterator) {
			if ((*fileIterator) == "v") {

				currentVertexesNumber += 1;
				++fileIterator;
				float val1 = std::stof((*fileIterator)) * inScale;
				++fileIterator;
				float val2 = std::stof((*fileIterator)) * inScale;
				++fileIterator;
				float val3 = std::stof((*fileIterator)) * inScale;
				if (isFirst) {
					PosX = val1;
					NegX = val1;
					PosY = val2;
					NegY = val2;
					PosZ = val3;
					NegZ = val3;
					isFirst = false;
				}
				if (val1 > PosX) {
					PosX = val1;
				}
				if (val2 > PosY) {
					PosY = val2;
				}
				if (val3 > PosZ) {
					PosZ = val3;
				}
				if (val1 < NegX) {
					NegX = val1;
				}
				if (val2 < NegY) {
					NegY = val2;
				}
				if (val3 < NegZ) {
					NegZ = val3;
				}
				Vertices.push_back(glm::vec3(val1, val2, val3));
			}
			else if ((*fileIterator) == "f") {
				int vertices[3] = { 0,0,0 };
				for (int k = 0; k < 3; k++) {
					++fileIterator;
					char curChar = ' ';
					std::string current = "";
					int i = 0;
					while (curChar != '/') {
						curChar = (*fileIterator)[i];
						current += curChar;
						i++;
					}
					vertices[k] = std::stoi(current) - lastMaxVertexNumber;
				}

				Indices.push_back(vertices[0]);
				Indices.push_back(vertices[1]);
				Indices.push_back(vertices[2]);
			}
			else if ((*fileIterator) == "o") {
				lastMaxVertexNumber = currentVertexesNumber + 1;
				mBodies.insert(std::make_unique<Body>(Vertices, Indices, this));
				Vertices.clear();
				Indices.clear();
			}


			++fileIterator;

		}
		lastMaxVertexNumber = currentVertexesNumber;
		mBodies.insert(std::make_unique<Body>(Vertices, Indices, this));
		Vertices.clear();
		Indices.clear();

		mBoundingBox.push_back(glm::vec3(PosX, PosY, PosZ));
		mBoundingBox.push_back(glm::vec3(PosX, PosY, NegZ));
		mBoundingBox.push_back(glm::vec3(PosX, NegY, PosZ));
		mBoundingBox.push_back(glm::vec3(PosX, NegY, NegZ));
		mBoundingBox.push_back(glm::vec3(NegX, PosY, PosZ));
		mBoundingBox.push_back(glm::vec3(NegX, PosY, NegZ));
		mBoundingBox.push_back(glm::vec3(NegX, NegY, PosZ));
		mBoundingBox.push_back(glm::vec3(NegX, NegY, NegZ));

		glm::vec3 currentSum = mBoundingBox[0];
		for (int i = 1; i < mBoundingBox.size(); i++) {
			currentSum += mBoundingBox[i];
		}

		mCurCm = currentSum / (float)mBoundingBox.size();
		mOriginalCm = mCurCm;
		mCurCm = inLocation;
		auto identity = glm::mat4(1);
		mCurPosMatrix = glm::translate(identity, mCurCm);
		file.close();
	}
}
