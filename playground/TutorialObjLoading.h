#pragma once

#include "ITutorialRenders.h"

#include <vector>
#include <glm/glm.hpp> // Include GLM (MATH)

#include "FreeCamera.h"
#include "SimpleGeometry.h"

using namespace atrng;
using namespace std;
using namespace glm;

class TutorialObjLoading : public ITutorialRenders
{
public:
    void Render_Loop_Setup(GLFWwindow*);
    void Render_Loop_Implementation();
    void Render_Loop_Cleanup();
protected:
private:
    GLuint VertexArrayID, MatrixID, ViewMatrixID, ModelMatrixID, Texture, TextureID,
           LightID, vertexbuffer, uvbuffer, elementbuffer, normalbuffer,
           programID;

    GLuint vertexbuffer2, uvbuffer2, normalbuffer2;

    FreeCamera* m_freeCamera;

    vector<unsigned short> m_indices;
    vector<vec3> m_vertices;
};