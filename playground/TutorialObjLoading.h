#pragma once

#include "ITutorialRenders.h"

#include <vector>
#include <glm/glm.hpp> // Include GLM (MATH)

#include "FreeCamera.h"
#include "Font2dLoader.h"
#include "Billboard.h"
#include "ParticleSystem.h"

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
           vertexbuffer, uvbuffer, normalbuffer, elementbuffer,
           LightID, 
           programID;

    GLuint vertexbuffer2, uvbuffer2, normalbuffer2, // non indexed vbo.
        //MatrixID2, ModelMatrixID2, ViewMatrixID2 /* THIS WAS KEY FOR THE SECOND OBJECT('s Lighting) */,
        //TextureID2, LightID2,
        transProgramID;

    FreeCamera*     m_freeCamera;
    Font2dLoader*   m_fontLoader;
    Billboard*      m_billboard;
    ParticleSystem* m_particles;

    vector<unsigned short> m_indices;
    vector<vec3> m_vertices;
};