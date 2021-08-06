#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

//카메라 이동에 대한 몇 가지 가능한 옵션을 정의합니다. 윈도우 시스템 특정 입력 방법으로부터 멀리하기 위해 추상화.
enum Camera_Movement
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
};

//기본 카메라 값
const float YAW			= -90.0f;
const float PITCH		=  0.0f;
const float SPEED		=  2.5f;
const float SENSITIVITY =  0.1f;
const float ZOOM		=  45.0f;

//입력을 처리하고 OpenGL에서 사용하기 위한 해당 오일러 각도, 벡터 및 행렬을 계산하는 추상 카메라 클래스
class Camera
{
public:
	// 카메라 속성
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;

	// 오일러 각도
	float Yaw;
	float Pitch;

	// 카메라 옵션
	float MovementSpeed;
	float MouseSensitivity;
	float Zoom;

	// 벡터가 있는 생성자
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH)
		: Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		Position = position;
		WorldUp = up;
		Yaw = yaw;
		Pitch = pitch;

		updateCameraVectors();
	};

	// 스칼라 값이 있는 생성자
	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
		:Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		Position = glm::vec3(posX, posY, posZ);
		WorldUp = glm::vec3(upX, upY, upZ);
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	};

	void SetMovementSpeed(float speed)
	{
		MovementSpeed = speed;
	};

	// 오일러 각도 및 LookAt 행렬을 사용하여 계산된 뷰 행렬을 반환합니다.
	virtual glm::mat4 GetViewMatrix()
	{
		return glm::lookAt(Position, Position + Front, Up);
	};

	// 키보드와 같은 입력 시스템에서 받은 입력을 처리합니다. 카메라 정의 ENUM 형식의 입력 매개 변수를 허용합니다(윈도우 시스템에서 추상화하기 위해).
	virtual void ProcessKeyboard(Camera_Movement direction, float deltaTime)
	{
		float velocity = MovementSpeed * deltaTime;
		if (direction == FORWARD)
			Position += Front * velocity;
		if (direction == BACKWARD)
			Position -= Front * velocity;
		if (direction == LEFT)
			Position -= Right * velocity;
		if (direction == RIGHT)
			Position += Right * velocity;
	};

	// 마우스 입력 시스템에서 받은 입력을 처리합니다. x 및 y 방향 모두에서 오프셋 값을 예상합니다.
	virtual void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
	{
		xoffset *= MouseSensitivity;
		yoffset *= MouseSensitivity;

		Yaw += xoffset;
		Pitch += yoffset;

		//피치가 범위를 벗어 났을 때 화면이 뒤집히지 않도록 합니다.
		if (constrainPitch)
		{
			if (Pitch > 89.0f)
				Pitch = 89.0f;
			if (Pitch < -89.0f)
				Pitch = -89.0f;
		}

		//업데이트 된 오일러 각도를 사용하여 전방, 오른쪽 및 위쪽 벡터 업데이트
		updateCameraVectors();
	};

	// 마우스 스크롤 휠 이벤트에서 받은 입력을 처리합니다. 수직 휠 축에 대한 입력만 필요합니다.
	virtual void ProcessMouseScroll(float yoffset) {
		Zoom -= (float)yoffset;
		if (Zoom < 1.0f)
			Zoom = 1.0f;
		if (Zoom > 45.0f)
			Zoom = 45.0f;
	};
private:
	//카메라의 (업데이트 된) 오일러 각도에서 전방 벡터를 계산합니다.
	virtual void updateCameraVectors()
	{
		// 새로운 전방 벡터 계산
		glm::vec3 front;
		front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		front.y = sin(glm::radians(Pitch));
		front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		Front = glm::normalize(front);

		//또한 오른쪽 및 윗쪽 벡터를 다시 계산합니다.
		Right = glm::normalize(glm::cross(Front, WorldUp)); // 벡터를 정규화하십시오. 그 길이가 0에 가까워 질수록 위나 아래를 더 많이 볼 수록 움직임이 느려지기 때문입니다.
		Up = glm::normalize(glm::cross(Right, Front));
	};
};

#endif