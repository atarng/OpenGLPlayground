#pragma once
#include <GL/glew.h>
#include <vector>
#include <glfw3.h>

#include "ICamera.h"
#include "ShaderReader.h"

using namespace std;
using namespace atrng;

class Billboard {
public:
    Billboard();
    void Initialize(ICamera* camera, bool fixedSize, bool alignY = true);
    void Render();
    void Cleanup();

    void SetPosition(vec3);
    void SetScale(vec2);

protected:
private:
    //bool   m_fixedSize;
    string m_billboardTexture;
    vec3   m_billboardPos;
    vec2   m_billboardScl; // 2d
    ShaderReader m_sr;

    bool   m_alignY;

////

    ICamera* m_camera;

    GLuint m_programID;
    GLuint m_billboardPosID, m_billboardSizeID;
    GLuint m_texture, m_textureID;

    GLuint m_vertex_buffer; // billboard_

    GLuint m_CameraUp_worldspace_ID, m_CameraRight_worldspace_ID;  
    GLuint m_ViewProjMatrixID;

    //vector<GLfloat> m_vertex_buffer_data;
    GLfloat* m_vertex_buffer_data;

    // TEMP
    GLuint t_LifeLevelID;
};