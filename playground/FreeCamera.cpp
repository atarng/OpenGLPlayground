#include "FreeCamera.h"
#include <glm/gtc/matrix_transform.hpp>


FreeCamera::FreeCamera(GLFWwindow* window) :
    m_position(0, 0, 5)
  , m_horizontalAngle(3.14f)
  , m_verticalAngle(0.0f)
  , m_fov(45.0f)
  , m_speed(3.0f)
  , m_mouseSpeed(0.005f)
{
    m_window = window;
}

// Temp function
void FreeCamera::Update()
{
    // glfwGetTime is called only once, the first time this function is called
    static double lastTime = glfwGetTime();

    // Compute time difference between current and last frame
    double currentTime = glfwGetTime();
    float deltaTime = float(currentTime - lastTime);

    // Get mouse position
    double xpos, ypos;
    glfwGetCursorPos(m_window, &xpos, &ypos);

    // Reset mouse position for next frame
    glfwSetCursorPos(m_window, 1024 / 2, 768 / 2);

    // Compute new orientation
    m_horizontalAngle += m_mouseSpeed * float(1024 / 2 - xpos);
    m_verticalAngle     += m_mouseSpeed * float(768 / 2 - ypos);

    // Direction : Spherical coordinates to Cartesian coordinates conversion
    glm::vec3 direction(
        cos(m_verticalAngle) * sin(m_horizontalAngle),
        sin(m_verticalAngle),
        cos(m_verticalAngle) * cos(m_horizontalAngle)
    );

    // Right vector
    glm::vec3 right = glm::vec3(
        sin(m_horizontalAngle - 3.14f / 2.0f),
        0,
        cos(m_horizontalAngle - 3.14f / 2.0f)
    );

    // Up vector
    glm::vec3 up = glm::cross(right, direction);

    // Move forward
    if( glfwGetKey(m_window, GLFW_KEY_UP) == GLFW_PRESS ||
        glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS
    ) {
        m_position += direction * deltaTime * m_speed;
    }
    // Move backward
    if( glfwGetKey(m_window, GLFW_KEY_DOWN) == GLFW_PRESS ||
        glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS
    ) {
        m_position -= direction * deltaTime * m_speed;
    }
    // Strafe right
    if (glfwGetKey(m_window, GLFW_KEY_RIGHT) == GLFW_PRESS ||
        glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS
    ) {
        m_position += right * deltaTime * m_speed;
    }
    // Strafe left
    if (glfwGetKey(m_window, GLFW_KEY_LEFT) == GLFW_PRESS ||
        glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS
    ) {
        m_position -= right * deltaTime * m_speed;
    }
    // Strafe Up
    if (glfwGetKey(m_window, GLFW_KEY_Q) == GLFW_PRESS) {
        m_position += up * deltaTime * m_speed;
    }
    // Strafe Down
    if( glfwGetKey(m_window, GLFW_KEY_Z) == GLFW_PRESS ) {
        m_position -= up * deltaTime * m_speed;
    }

    float FoV = m_fov;// - 5 * glfwGetMouseWheel(); // Now GLFW 3 requires setting up a callback for this. It's a bit too complicated for this beginner's tutorial, so it's disabled instead.

                           // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
    m_projectionMatrix = perspective(FoV, 4.0f / 3.0f, 0.1f, 100.0f);
    // Camera matrix
    m_viewMatrix = lookAt(
        m_position,             // Camera is here
        m_position + direction, // and looks here : at the same position, plus "direction"
        up                      // Head is up (set to 0,-1,0 to look upside-down)
    );

    // For the next frame, the "last time" will be "now"
    lastTime = currentTime;
}