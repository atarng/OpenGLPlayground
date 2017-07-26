#pragma once

#include "ITutorialRenders.h"

#include <vector>
#include <glm/glm.hpp> // Include GLM (MATH)

#include "FreeCamera.h"

using namespace atrng;
using namespace std;
using namespace glm;

class TutorialBumpMapping : public ITutorialRenders
{
public:
    void Render_Loop_Setup(GLFWwindow* window);
    void Render_Loop_Implementation();
    void Render_Loop_Cleanup();
protected:
private:
    GLuint VertexArrayID, MatrixID, ViewMatrixID, ModelMatrixID, ModelView3x3MatrixID,
        DiffuseTexture,   NormalTexture,   SpecularTexture, 
        DiffuseTextureID, NormalTextureID, SpecularTextureID,
        vertexbuffer, uvbuffer, normalbuffer, elementbuffer, tangentbuffer, bitangentbuffer,
        LightID,
        programID;

    FreeCamera*   m_freeCamera;


    vector<unsigned short> m_indices;
    vector<vec3> indexed_vertices, indexed_normals, indexed_tangents, indexed_bitangents;
};