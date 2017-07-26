#pragma once

#include "ITutorialRenders.h"

#include <vector>
#include <glm/glm.hpp> // Include GLM (MATH)

#include "FreeCamera.h"

using namespace atrng;
using namespace std;
using namespace glm;

class TutorialRenderToTexture: public ITutorialRenders
{
public:
    void Render_Loop_Setup(GLFWwindow*);
    void Render_Loop_Implementation();
    void Render_Loop_Cleanup();
protected:
private:
    int m_windowWidth, m_windowHeight;

    GLuint VertexArrayID, programID, MatrixID, ViewMatrixID, ModelMatrixID,
        Texture, TextureID, LightID,
        vertexbuffer, uvbuffer, normalbuffer, elementbuffer,
        quad_vertexbuffer, FramebufferName, renderedTexture,
        quad_programID, texID, timeID, depthrenderbuffer;

    FreeCamera*   m_freeCamera;

    vector<unsigned short> m_indices;
};