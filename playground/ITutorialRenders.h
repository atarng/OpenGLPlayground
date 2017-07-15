#pragma once 

#include <GL/glew.h>   // Include GLEW
#include <glfw3.h>     // Include GLFW

namespace atrng{
class ITutorialRenders {
public:
    virtual void Render_Loop_Setup(GLFWwindow*) = 0;
    virtual void Render_Loop_Implementation() = 0;
    virtual void Render_Loop_Cleanup() = 0;
};
}