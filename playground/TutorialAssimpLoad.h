#pragma once
#include "ITutorialRenders.h"

#include "AssimpWrapper.h"
#include "FreeCamera.h"

class TutorialAssimpLoad : public ITutorialRenders {
public:
    void Render_Loop_Setup(GLFWwindow* window);
    void Render_Loop_Implementation();
    void Render_Loop_Cleanup();
protected:

private:
    AssimpWrapper* m_assimpW;
    FreeCamera*    m_freeCamera;

    GLuint programID, VertexArrayID, Texture, TextureID;
    GLuint MatrixID, ModelMatrixID, ViewMatrixID;
    GLuint LightID;
};