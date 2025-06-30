#include "../../headers/rendering/MainRenderer.h"
#include <stdlib.h>
MainRenderer::MainRenderer()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	mWindow = glfwCreateWindow(800, 800, "YoutubeOpenGL", NULL, NULL);
	if (mWindow == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
	}
	glfwMakeContextCurrent(mWindow);
	gladLoadGL();
	glViewport(0, 0, 800, 800);
	glEnable(GL_DEPTH_TEST);
	glLineWidth(3.0f);

	mShaders.push_back(ShaderClass("./shaders/default.vert", "./shaders/default.geom", "./shaders/default.frag"));
	mShaders.push_back(ShaderClass("./shaders/default.vert", "./shaders/outline.geom", "./shaders/outline.frag"));

}
void
MainRenderer::Render()
{
	if(glfwWindowShouldClose(mWindow)){ exit(3); }
	glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	DrawObjects();
	glfwSwapBuffers(mWindow);
	glfwPollEvents();
}
void
MainRenderer::DrawObjects()
{
	for (auto& shader : mShaders)
	{
		shader.Activate();
		for (auto mesh : mMeshes)
		{
			mesh->Draw(shader);
		}
	}
}
