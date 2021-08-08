#pragma once
#include <MyGui.h>


class DebugGUI :
    public MyGui
{
public:
    DebugGUI(GLFWwindow* window);
    virtual void CreateWindow();

    float* GetLightColor();
    float* GetObjectColor();
    float GetAmbientStrength();
    float GetSpecularStrength();
    int GetShininess();

private:
    float lightColor[3];
    float objectColor[3];
    float ambientStrength;
    float specularStrength;
    int shininess;
};

