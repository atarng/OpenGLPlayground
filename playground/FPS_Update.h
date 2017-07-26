#pragma once
#include <glm/glm.hpp> // Include GLM (MATH)
#include <GL/glew.h>   // Include GLEW
#include <glfw3.h>     // Include GLFW
#include <stdio.h>

using namespace glm;
class FPS_Update {
public:
    FPS_Update();
    void Loop();

private:
// For speed computation
    double lastTime;
    int nbFrames;
};