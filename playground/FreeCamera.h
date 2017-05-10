#pragma once

#include <glfw3.h>
#include <glm/glm.hpp> // Include GLM (MATH)

#include "ICamera.h"
#include "IInputListener.h"

using namespace glm;
using namespace atrng;

class FreeCamera : public ICamera, public IInputListener
    //, public IUpdate
{
public:
    FreeCamera(GLFWwindow*);

    void Update();

    inline mat4 GetProjectionMatrix() { return m_projectionMatrix; }
    inline mat4 GetViewMatrix() { return m_viewMatrix; }

protected:
private:

    // Initial position : on +Z
    vec3 m_position;
    // Initial horizontal angle : toward -Z
    float m_horizontalAngle;
    // Initial vertical angle : none
    float m_verticalAngle;
    // Initial Field of View
    float m_fov;

    float m_speed; // 3 units / second
    float m_mouseSpeed;

    GLFWwindow* m_window;

    mat4 m_projectionMatrix;
    mat4 m_viewMatrix;

};