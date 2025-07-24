#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include "headers/OpenGL/VAO.h"
#include "headers/OpenGL/EBO.h"
#include "headers/controls/Camera.h"
#include "headers/GlobalVariables.h"
#include "headers/rendering/MainRenderer.h"
#include "headers/core/MainEngine.h"
#include "headers/modeling/Delaunay.h"
#include "headers/modeling/MyMath.h"
#include "headers/testing/LineIntersectionTesting.h"
#include "headers/modeling/TwoDMeshContainer.h"'
#include <functional>'
#include "headers/core/GlobalVariables.h"
int main()
{

	MainRenderer renderer;
	MainEngine   mainEngine;
	Camera       mainCamera(SCREEN_WIDTH, SCREEN_HEIGH, 1, glm::vec3(3.5f, 3.5f, 13.0f));
	Delaunay     del;

	debugFunc = [&](std::vector<MyVec2> inVec, double inScale)
		{
			for (auto& vec : inVec)
			{
				Mesh* testCube = new Mesh("./models/cube.object", inScale, glm::vec3(vec.x, vec.y, 0));
				renderer.AddMesh(testCube);
			}
		};

	auto testMesh = del.Create2DConstrainedNew("./models/testButerflyNew.delaunay", 0.005, 0.01);

	renderer.AddMesh(testMesh);

	mainCamera.setMatrixView();
	for (auto& shader : renderer.GetShaders())
	{
		mainCamera.Matrix(shader);
	}
	mainEngine.AddOnUpdate(&MainRenderer::Render, &renderer);
	mainEngine.AddOnUpdate([&]()
		{
			mainCamera.Inputs(renderer.GetWindow());
			mainCamera.setMatrixView();
			for (auto& shader : renderer.GetShaders())
			{
				mainCamera.Matrix(shader);
			}
		});
	mainEngine.BeginExecution();
	return 0;
}