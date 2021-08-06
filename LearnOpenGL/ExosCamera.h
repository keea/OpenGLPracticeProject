#pragma once
#include "Camera.h"
class ExosCamera : public Camera
{
public:
	ExosCamera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH)
		:Camera(position, up, yaw, pitch)
	{
		MovementSpeed = 100.0f;
	};

	ExosCamera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
		:Camera(posX, posY, posZ, upX, upY, upZ, yaw, pitch)
	{
		MovementSpeed = 100.0f;
	};

	void SetCameraPosition(glm::vec3 position)
	{
		Position = position;
	};
};

