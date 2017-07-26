#pragma once

#include "ITutorialRenders.h"

#include <vector>
#include <glm/glm.hpp> // Include GLM (MATH)
#include <glfw3.h>

#include "FreeCamera.h"
#include "Font2dLoader.h"

using namespace atrng;
using namespace std;
using namespace glm;

class TutorialExtensions : public ITutorialRenders
{
public:
    void Render_Loop_Setup(GLFWwindow*);
    void Render_Loop_Implementation();
    void Render_Loop_Cleanup();

protected:
    //void APIENTRY DebugOutputCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);

private:
    int nbFrames;
    double lastTime;

    GLuint VertexArrayID, MatrixID, ViewMatrixID, ModelMatrixID,
        Texture, TextureID,
        vertexbuffer, uvbuffer, normalbuffer, elementbuffer,
        LightID,
        programID;

    GLuint vertexbuffer2, uvbuffer2, normalbuffer2, // non indexed vbo.
        transProgramID;

    FreeCamera*   m_freeCamera;
    Font2dLoader* m_fontLoader;

    vector<unsigned short> m_indices;
    //vector<vec3> m_vertices;
};