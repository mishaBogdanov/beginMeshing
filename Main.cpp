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
#include "headers/modeling/TwoDMeshContainer.h"

int main()
{
	//glm::vec2 p0 = { 0,0 }, p1 = { 1,1 }, p2 = { 0.5, 0.5 }, p3 = { 0,2 };
	//auto ans = GetLineLineIntersection(p0, p1, p2, p3);
	//glm::vec2 p4 = { 0,0 }, p5 = { 0,1 }, p6 = { 1,2 };
	//auto p = GetCenterTriangle(p4, p5, p6);
	//LineIntersectionTesting t;
	//t.runTest();
	TwoDMeshContainer t;
	t.addTriangle({1,2,3});
	t.removeTriangle({ 1,2,3 });


	MainRenderer renderer;
	MainEngine   mainEngine;
	Camera       mainCamera(SCREEN_HEIGH, SCREEN_WIDTH, 1, glm::vec3(3.5f, 3.5f, 13.0f));
	//Mesh         testCube("./models/helicopter.object", 10, glm::vec3(0,0,0));
	//std::vector<std::vector<glm::vec3>> f = { {{0,0,0},{100,0,0},{0,100,0}} };
	//std::vector<std::vector<GLuint>> k = { {0,1,2} };


	Delaunay del;
	auto testMesh = del.Create2DConsSmoothedWEdge("./models/testButerfly.delaunay", 0.5, 4, 4);



	//auto testMesh = del.Create2DConstrained("./models/testFish.delouney");
	//Mesh         testMesh(f,k);
	//renderer.AddMesh(&testCube);



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