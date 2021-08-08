#include <iostream>
#include<glad/glad.h>
#include <GLFW/glfw3.h>

#include "Shader.h" 
#include "Camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "DrawSphere.h"

#include "GUI/DebugGUI.h"


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

//setting
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

//카메라
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = 400, lastY = 300;
bool firstMouse = true;

//타이밍
float deltaTime = 0.0f; //현재 프레임과 마지막 프레임 사이의 시간
float lastFrame = 0.0f;

glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

int main()
{
	glfwInit(); //glfw 초기화

	/* 3.3버전을 사용한다는 것을 알림 */
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //glfw에게 core-profile을 사용한다고 알림.

	/* 윈도우 객체 생성 */
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Phong_Lighting_Model", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window); //window의 컨텍스트를 현재 스레드의 주 컨텍스트로 지정하겠다고 알림.
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);


	/* GLAD 초기화 */
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glEnable(GL_DEPTH_TEST);

	Shader lightingShader = Shader(2, "ShaderFiles/basic_lighting.vs", "ShaderFiles/basic_lighting.fs");

	// 구 오브젝트 생성
	DrawSphere sphereMesh;

	DebugGUI myGui(window);

	/* render loop */
	while (!glfwWindowShouldClose(window)) //GLFW가 종료하도록 지시 되었는지 확인
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		/* input */
		processInput(window);

		/* render */
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		myGui.StartFrame();
		myGui.CreateWindow();

		float* lightColor = myGui.GetLightColor();
		float* objectColor = myGui.GetObjectColor();

		// 유니폼 / 그릴 오브젝트를 설정할 때 셰이더를 활성화 해야합니다.
		lightingShader.use();
		lightingShader.setVec3("objectColor", objectColor[0], objectColor[1], objectColor[2]);
		lightingShader.setVec3("lightColor", lightColor[0], lightColor[1], lightColor[2]);
		lightingShader.setVec3("lightPos", lightPos);
		lightingShader.setVec3("viewPos", camera.Position);

		lightingShader.setFloat("ambientStrength", myGui.GetAmbientStrength());
		lightingShader.setFloat("specularStrength", myGui.GetSpecularStrength());
		lightingShader.setInt("shininess", myGui.GetShininess());

		// 보기/투영 변환
		glm::mat4 projection;
		projection = glm::perspective(glm::radians(camera.Zoom), 800.0f / 600.0f, 0.1f, 100.0f);
		lightingShader.setMat4("projection", projection);
		glm::mat4 view = camera.GetViewMatrix();
		lightingShader.setMat4("view", view);

		// 월드 변환
		glm::mat4 model = glm::mat4(1.0f);
		lightingShader.setMat4("model", model);

		// 구를 렌더링
		sphereMesh.Draw();

		myGui.Rendering();

		glfwSwapBuffers(window); //컬러 버퍼를 교체
		glfwPollEvents(); //이벤트가 발생했는지 확인하고 윈도우 상태 업데이트하며 정해진 함수 호출.
	}

	sphereMesh.Release();

	glfwTerminate(); //할당된 모든 자원들을 정리/삭제.

	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	std::cout << "call framebuffer_size_callback" << std::endl;
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) //ESC 키를 눌렀는지 확인
	{
		std::cout << "ESC 눌려짐" << std::endl;

		glfwSetWindowShouldClose(window, true); //WindowShouldClose 속성을 true 세팅
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(FORWARD, deltaTime);
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(LEFT, deltaTime);
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(RIGHT, deltaTime);
	}

}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE)
	{
		firstMouse = true;

		return;
	}

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}


	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}