#pragma once

// 3rdParty Headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>

void inputInit(GLFWwindow *window);

void inputLoop(
    GLFWwindow *window,
    float frameDeltaTime,
    float *horizontalDeltaRad,
    float *verticalDeltaRad,
    float *deltaScaleNorm
);
