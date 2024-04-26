#include "GL_camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include<iostream>


GL_camera::GL_camera(const glm::vec3& position,const glm::vec3& front,const glm::vec3& up) :camera_position(position), camera_front(front), camera_up(up),yaw(YAW),pitch(PITCH) ,World_up(WORLD_UP),Zoom(ZOOM){
	first = true;
}
glm::mat4 GL_camera::GetViewMatrix() const {

	//glm::lookat(�����λ�ã�target,up_dir)
	return glm::lookAt(camera_position, camera_position+camera_front, World_up);
}


void  GL_camera::ProcessKeyboard(Camera_Movement direction, float deltaTime) {

	//float speed = 2.5 * deltaTime;
	//diagnal length control


	float speed = steplength*deltaTime ;
	std::cout << steplength <<" "<< deltaTime << "  " << 2.5 * deltaTime << std::endl;


	if (direction == FORWARD) {
		camera_position += camera_front * speed;
	}
	if (direction == BACKWARD) {
		camera_position -= camera_front * speed;
	}
	if (direction == LEFT) {
		camera_position -= glm::normalize(glm::cross(camera_front, World_up)) * speed;
	}
	if (direction == RIGHT) {
		camera_position += glm::normalize(glm::cross(camera_front, World_up)) * speed;
	}
}


void  GL_camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch) {
	const float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;
	yaw += xoffset;
	pitch += yoffset;
	if (constrainPitch) {
		if (pitch > 89.0f) {
			pitch = 89.0f;
		}
		if (pitch < -89.0f) {
			pitch = -89.0f;
		}

	}
	updateCameraVectors();

}

void GL_camera::updateCameraVectors() { 
	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	camera_front = glm::normalize(direction);
	right = glm::normalize(glm::cross(camera_front, World_up));
	camera_up= glm::normalize(glm::cross(right,camera_up));
}

void GL_camera::setCameraVectors(glm::vec3 direction) { 
	camera_front = glm::normalize(direction);
	right = glm::normalize(glm::cross(camera_front, World_up));
	camera_up= glm::normalize(glm::cross(right,camera_up));
}


void GL_camera::ProcessMouseScroll(float yoffset) {

	if (Zoom >= 1.0f && Zoom <= 45.0f) {
		Zoom -= (float)yoffset;
	}
	if (Zoom < 1.0f)
		Zoom = 1.0f;
	if (Zoom > 45.0f)
		Zoom = 45.0f;
}

float GL_camera::getZoomValue() const{
	return Zoom;
}

glm::vec3  GL_camera::getCameraPos() const  {
	return camera_position;
}

void GL_camera::set_CameraPos(glm::vec3 CamPos) {
	camera_position = CamPos;
	// std::cout << "set Camera Pos " << camera_position.x << " " << camera_position.y << " " << camera_position.z << " " << std::endl;
}

