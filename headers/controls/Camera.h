#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "../OpenGL/ShaderClass.h"
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/rotate_vector.hpp>


struct doubleVec3
{
	glm::vec3 position, direction;
};

class Camera
{
private:
	glm::vec3 position;
	glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);
	float fov;
	int width;
	int height;
	float speed;
	float sensitivity = 200.0f;
	float xScale;
	double currentTime;
	glm::mat4 camMatrix;
	glm::mat4 translationMatrix;
	float nearPlane;
	float farPlane;
	float distFromScreen;

	void getDistFromScreen();
public:
	Camera();
	Camera(int widht, int height, float gxScale, glm::vec3 gPosition);

	void setMatrix(float FOVdeg, float nearPlane, float farPlane);
	void setMatrixView();

	void Inputs(GLFWwindow* window);
	void Matrix(ShaderClass& shader);
	void setView(GLFWwindow* window, glm::vec3 camPos, glm::vec3 lookTo);
	void resetMousePosition(GLFWwindow* window);
	void getMousePosition(glm::vec3& pos, glm::vec3& direction, GLFWwindow* window);
	doubleVec3 get3DMouse();
};

