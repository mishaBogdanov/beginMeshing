#pragma once
#include <vector>
#include "../OpenGL/ShaderClass.h"
#include "../modeling/Mesh.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
class MainRenderer
{
public:
	MainRenderer();

	void DrawObjects();
	void Render();
	void AddMesh(Mesh* inMesh) { mMeshes.push_back(inMesh); }
	GLFWwindow* GetWindow() { return mWindow; }
	std::vector<ShaderClass>& GetShaders() { return mShaders; }
private:
	GLFWwindow*              mWindow;
	std::vector<ShaderClass> mShaders;
	std::vector<Mesh*>       mMeshes;
};

