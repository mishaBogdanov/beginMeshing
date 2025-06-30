#include "../../headers/controls/Camera.h"
#include "../../headers/GlobalVariables.h"
#include<glm/gtx/vector_angle.hpp>
#include "Windows.h"


Camera::Camera(int gWidth, int gHeight, float gxScale, glm::vec3 gPosition) {

	position = gPosition;
	Orientation = glm::vec3(0, 0, -1);
	width = gWidth;
	height = gHeight;
	xScale = gxScale;
	speed = 2;
	sensitivity = 200;
	currentTime = glfwGetTime();
	fov = 40.0f; 
	nearPlane = 0.0001f;
	farPlane = 200.0f;
	getDistFromScreen();
}

Camera::Camera() {

}

void Camera::setMatrix(float FOVdeg, float nearPlane, float farPlane) {
	translationMatrix = glm::lookAt(position, position + Orientation, Up);
	camMatrix = glm::perspective(glm::radians(FOVdeg), (float)width / height / xScale, nearPlane, farPlane);
}

void Camera::setMatrixView() {
	translationMatrix = glm::lookAt(position, position + Orientation, Up);
	camMatrix = glm::perspective(glm::radians(fov), (float)width / height / xScale, nearPlane, farPlane);
}

void Camera::getDistFromScreen() {
	distFromScreen = height / 2 / (glm::tan(glm::radians(fov/2)));
}


void Camera::Inputs(GLFWwindow* window) {
	double deltaT = glfwGetTime() - currentTime;
	currentTime = glfwGetTime();

	if (GetKeyState('W') & 0x8000) {
		position += (float)(deltaT * speed) * Orientation;
	}
	if (GetKeyState('S') & 0x8000) {
		position -= (float)(deltaT * speed) * Orientation;
	}
	if (GetKeyState('A') & 0x8000) {
		position -= glm::normalize(glm::cross(Orientation, Up)) * (float)(deltaT * speed);
	}
	if (GetKeyState('D') & 0x8000) {
		position += glm::normalize(glm::cross(Orientation, Up)) * (float)(deltaT * speed);
	}
	if (GetKeyState(VK_SPACE) & 0x8000) {
		position += Up * (float)(deltaT * speed);
	}
	if (GetKeyState(VK_LSHIFT) & 0x8000) {
		position -= Up * (float)(deltaT * speed);
	}
	static bool isMousePressed = false;
	static double mousePosX = 0.0f;
	static double mousePosY = 0.0f;

	if (GetKeyState(VK_LBUTTON) & 0x8000) {
		if (!isMousePressed) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
			glfwGetCursorPos(window, &mousePosX, &mousePosY);
			resetMousePosition(window);
			isMousePressed = true;
		}

		// Prevents camera from jumping on the first click


		// Stores the coordinates of the cursor
		double mouseX;
		double mouseY;
		// Fetches the coordinates of the cursor
		glfwGetCursorPos(window, &mouseX, &mouseY);

		// Normalizes and shifts the coordinates of the cursor such that they begin in the middle of the screen
		// and then "transforms" them into degrees 


		float rotX = (deltaT * sensitivity) * (float)(mouseY - (height / 2));
		float rotY = (deltaT * sensitivity) * (float)(mouseX - (width / 2));


		// Calculates upcoming vertical change in the Orientation
		glm::vec3 newOrientation = glm::rotate(Orientation, glm::radians(-rotX), glm::normalize(glm::cross(Orientation, Up)));

		// Decides whether or not the next vertical Orientation is legal or not
		if (abs(glm::angle(newOrientation, Up) - glm::radians(90.0f)) <= glm::radians(85.0f))
		{
			Orientation = newOrientation;
		}

		 //Rotates the Orientation left and right
		Orientation = glm::rotate(Orientation, glm::radians(-rotY), Up);


		// Sets mouse cursor to the middle of the screen so that it doesn't end up roaming around

		resetMousePosition(window);
	}
	else {
		if (isMousePressed) {
			glfwSetCursorPos(window, mousePosX, mousePosY);
			isMousePressed = false;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

		}
	}
}

void Camera::Matrix(ShaderClass& shader) {
	shader.Activate();
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "translationMatrix"), 1, GL_FALSE, glm::value_ptr(translationMatrix));
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "camMatrix"), 1, GL_FALSE, glm::value_ptr(camMatrix));

}



void Camera::setView(GLFWwindow* window, glm::vec3 pos, glm::vec3 lookTo) {
	position = pos;
	Orientation = glm::normalize(lookTo - pos);
	//resetMousePosition(window);
}

void Camera::resetMousePosition(GLFWwindow* window) {
	glfwSetCursorPos(window, (width / 2), (height / 2));
}

void Camera::getMousePosition(glm::vec3& pos, glm::vec3& direction, GLFWwindow* window) {
	pos = position;
	double mousePosX, mousePosY;
	glfwGetCursorPos(window, &mousePosX, &mousePosY);
	mousePosX -= width / 2;
	mousePosY = height / 2-mousePosY;
	mousePosX -= SCREEN_WIDTH / 2;
	mousePosY -= SCREEN_HEIGH / 2;

	//mousePosX -= SCR_WIDTH / 2 * offset[0];


	// naming as if we're facing in the posetive X direction
	// right will be the pos Y
	// up will be Z
	glm::vec3 Y = glm::normalize(glm::cross(Up, Orientation));
	glm::vec3 Z = glm::normalize(glm::cross(Orientation, Y));
	glm::mat3 orientationMatrix = { Orientation.x, Orientation.y, Orientation.z, Y.x, Y.y,Y.z, Z.x,Z.y, Z.z};
	glm::vec3 currentDirection = glm::normalize(glm::vec3({distFromScreen, -mousePosX, mousePosY}));	
	direction = orientationMatrix * currentDirection;
}





