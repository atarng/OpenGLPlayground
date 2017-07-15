#pragma once

#include "ITutorialRenders.h"

#include <vector>
#include <glm/glm.hpp> // Include GLM (MATH)

#include "FreeCamera.h"
#include "SimpleGeometry.h"

using namespace atrng;
using namespace std;
using namespace glm;

class TutorialShadowMapping : public ITutorialRenders
{
public:
    void Render_Loop_Setup(GLFWwindow*);
    void Render_Loop_Implementation();
    void Render_Loop_Cleanup();
protected:
private:
    FreeCamera* m_freeCamera;

////////
    int m_windowWidth, m_windowHeight;

    GLuint VertexArrayID, MatrixID, ViewMatrixID, ModelMatrixID,
           Texture, TextureID, LightID, 
           vertexbuffer, uvbuffer, normalbuffer, elementbuffer,
           programID;
    
    // Introduced during ShadowMapping
    GLuint DepthBiasID, depthMatrixID, lightInvDirID,
        FramebufferName,
        quad_vertexbuffer, depthTexture, ShadowMapID,
        depthProgramID, quad_programID, texID;

    //vector<vec2> m_uvs;
    //vector<vec3> m_vertices, m_normals;
    vector<unsigned short> m_indices;

};