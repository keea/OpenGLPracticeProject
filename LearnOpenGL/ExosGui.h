#pragma once
#include "MyGui.h"
#include <glm/glm.hpp>

class ExosGui :
    public MyGui
{
public:
    ExosGui(GLFWwindow* window, glm::vec3 cameraPosition)
        :MyGui(window)
    {
        isWireFrame = false;
        isSpin = false;
        SetCameraPosition(cameraPosition);
    };


    virtual void CreateWindow()
    {
        ImGui::Begin("Setting");

        ImGui::InputFloat3("Camera Position", CameraPosition);
        ImGui::Checkbox("WireFrameMode", &isWireFrame);
        ImGui::Checkbox("Use Spin", &isSpin);

        ImGui::End();
    };

    glm::vec3 GetCameraPosition()
    {
        return glm::vec3(CameraPosition[0], CameraPosition[1], CameraPosition[2]);
    };

    void SetCameraPosition(glm::vec3 cameraPosition)
    {
        CameraPosition[0] = cameraPosition.x;
        CameraPosition[1] = cameraPosition.y;
        CameraPosition[2] = cameraPosition.z;
    };

    bool GetIsWireFrame()
    {
        return isWireFrame;
    };

    bool GetIsSpin()
    {
        return isSpin;
    };

private:
    float CameraPosition[3];
    bool isWireFrame;
    bool isSpin;
};

