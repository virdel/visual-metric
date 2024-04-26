#pragma once
#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include<iostream>


enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

//��ʼ����
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;
const glm::vec3 WORLD_UP = glm::vec3(0.0, 1.0, 0.0);


class GL_camera
{

public:
	glm::vec3 camera_position,camera_front,camera_up,right,World_up;
	glm::mat4 view;
	float yaw, pitch,Zoom;
	bool first;

	float steplength;

	void setStepLength(float length) {

		//std::cout << "set Camera step length" << length << std::endl;
		steplength = length;
	}


	void updateCameraVectors();

public:
	GL_camera(const glm::vec3& position,const glm::vec3& front, const glm::vec3& up);
	GL_camera();
	void ProcessKeyboard(Camera_Movement direction, float deltaTime);
	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);
	void ProcessMouseScroll(float yoffset);
	glm::mat4 GetViewMatrix() const;
	float getZoomValue() const;
	glm::vec3 getCameraPos() const;
	void set_CameraPos(glm::vec3);
	void GL_camera::setCameraVectors(glm::vec3 direction);


};

