#include "DebugGUI.h"

DebugGUI::DebugGUI(GLFWwindow* window)
	:MyGui(window)
{
	lightColor[0] = 1.0f;
	lightColor[1] = 1.0f;
	lightColor[2] = 1.0f;

	objectColor[0] = 1.0f;
	objectColor[1] = 0.5f;
	objectColor[2] = 0.31f;

	ambientStrength = 0.1;
	specularStrength = 0.5f;
	shininess = 32;
}

void DebugGUI::CreateWindow()
{
	ImGui::Begin("Phong lighting model");
	
	// light Color : 빛 색상.
	ImGui::ColorEdit3("Light Color", lightColor);

	// object Color : 오브젝트 색상
	ImGui::ColorEdit3("Object Color", objectColor);

	// ambientStrength : 앰비언트 강도
	ImGui::SliderFloat("Ambient Strength", &ambientStrength, 0, 1);

	// speclarstergth : 스페큘러 강도
	ImGui::SliderFloat("Speclar Strength", &specularStrength, 0, 1);

	// shininess : 매끈함 정도
	ImGui::SliderInt("Shininess", &shininess, 1, 100);

	ImGui::End();
}

float* DebugGUI::GetLightColor()
{
	return lightColor;
}

float* DebugGUI::GetObjectColor()
{
	return objectColor;
}

float DebugGUI::GetAmbientStrength()
{
	return ambientStrength;
}

float DebugGUI::GetSpecularStrength()
{
	return specularStrength;
}

int DebugGUI::GetShininess()
{
	return shininess;
}
