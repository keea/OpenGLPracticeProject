#include<glad/glad.h>
#include <GLFW/glfw3.h>


#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Camera.h"
#include "Model.h"

#include "ExosCamera.h"
#include "ExosGui.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

//setting
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

//카메라
ExosCamera camera(glm::vec3(0.0f, 0.0f, 300.0f));
float lastX = 400, lastY = 300;
bool firstMouse = true;

//타이밍
float deltaTime = 0.0f; //현재 프레임과 마지막 프레임 사이의 시간
float lastFrame = 0.0f;


int main()
{
	glfwInit(); //glfw 초기화

	/* 3.3버전을 사용한다는 것을 알림 */
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //glfw에게 core-profile을 사용한다고 알림.

	/* 윈도우 객체 생성 */
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
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

	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	/* GLAD 초기화 */
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//stbi_set_flip_vertically_on_load(true);

	glEnable(GL_DEPTH_TEST);

	Shader ourShader = Shader("ShaderFiles/model_loading.vs", "ShaderFiles/model_loading.fs");

	//모델 로드
	Model ourModel("Res/objects/Baraka/Baraka.FBX");

	ExosGui myGui(window, camera.Position);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	/* render loop */
	while (!glfwWindowShouldClose(window)) //GLFW가 종료하도록 지시 되었는지 확인
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		/* input */
		processInput(window);
		myGui.SetCameraPosition(camera.Position);


		/* Gui Init */
		myGui.StartFrame();
		myGui.CreateWindow();


		/* render */
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (myGui.GetIsWireFrame())
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		ourShader.use();

		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
		glm::mat4 view = camera.GetViewMatrix();
		ourShader.setMat4("projection", projection);
		ourShader.setMat4("view", view);

		glm::mat4 model = glm::mat4(1.0f);

		//캐릭터 위치 변경. y축으로 100 내린다.
		model = glm::translate(model, glm::vec3(0.0f, -100.0f, 0.0f));
		//모델을 각도를 바꿈.
		if (myGui.GetIsSpin())
			model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));

		//시간에 따라 모델이 회전하도록 함
		//model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));

		ourShader.setMat4("model", model);

		ourModel.Draw(ourShader);

		camera.SetCameraPosition(myGui.GetCameraPosition());

		myGui.Rendering();

		/*  swap buffers and poll IO events */
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	myGui.CleanUp();

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