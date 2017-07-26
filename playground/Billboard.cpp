#include "Billboard.h"

#include "TextureLoader.h"

#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#define BB 12

Billboard::Billboard()
{
    m_billboardTexture = "ExampleBillboard.DDS";

    // texture vertex placements
    m_vertex_buffer_data = new GLfloat[BB]{ -0.5f, -0.5f, 0.0f,
                                             0.5f, -0.5f, 0.0f,
                                            -0.5f,  0.5f, 0.0f,
                                             0.5f,  0.5f, 0.0f, };
}

void Billboard::Initialize(ICamera* camera, bool fixedSize, bool alignY) {
    m_camera = camera;
    m_alignY = alignY;


    TextureLoader tl;

    // these are going to be dependent on what we use in our shader.
    if (fixedSize) {
        m_programID = m_sr.LoadShaders("Billboard_Fixed.vertexshader", "Billboard.fragmentshader");
    }
    else {
        m_programID = m_sr.LoadShaders("Billboard.vertexshader", "Billboard.fragmentshader");

        m_CameraUp_worldspace_ID    = glGetUniformLocation(m_programID, "CameraUp_worldspace");
        m_CameraRight_worldspace_ID = glGetUniformLocation(m_programID, "CameraRight_worldspace");
    }

    m_ViewProjMatrixID          = glGetUniformLocation(m_programID, "VP");

    m_billboardPosID  = glGetUniformLocation(m_programID, "BillboardPos");
    m_billboardSizeID = glGetUniformLocation(m_programID, "BillboardSize");

    m_textureID = glGetUniformLocation(m_programID, "myTextureSampler");
    vector<string> vs;
    vs.push_back(m_billboardTexture);
    m_texture = tl.LoadFiles(vs)[0];

    ////
    t_LifeLevelID = glGetUniformLocation(m_programID, "LifeLevel");
    m_billboardPos = { 0.0f, 1.25f, 0.0f };
    m_billboardScl = { 1.0f, 0.125f };

    glGenBuffers(1, &m_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, BB * sizeof(GLfloat), m_vertex_buffer_data, GL_DYNAMIC_DRAW);
}

void Billboard::SetPosition(vec3 pos) {
    m_billboardPos = pos;
}
void Billboard::SetScale(vec2 scl) {
    m_billboardScl = scl;
}

void Billboard::Render() {

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram(m_programID);

    // Bind our texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    // Set our "myTextureSampler" sampler to user Texture Unit 0
    glUniform1i(m_textureID, 0);

    mat4 ViewMatrix           = m_camera->GetViewMatrix();
    mat4 ViewProjectionMatrix = m_camera->GetProjectionMatrix() * ViewMatrix;


    // VM:
    // RIGHT [(0,0), (0,1), (0,2), (0,3)
    //        (1,0), (1,1), (1,2), (1,3)
    //        (2,0), (2,1), (2,2), (2,3)
    //        (3,0), (3,1), (3,2), (3,3)]


    // This is the only interesting part of the tutorial.
    // This is equivalent to mlutiplying (1,0,0) and (0,1,0) by inverse(ViewMatrix).
    // ViewMatrix is orthogonal (it was made this way), so its inverse is also its transpose, 
    // and transposing a matrix is "free" (inversing is slooow)
    if(m_alignY){
        glUniform3f(m_CameraRight_worldspace_ID, ViewMatrix[0][0], ViewMatrix[1][0], ViewMatrix[2][0]);
        glUniform3f(m_CameraUp_worldspace_ID,    ViewMatrix[0][1], ViewMatrix[1][1], ViewMatrix[2][1]);
    }
    else{
        vec3 camPosition_y = m_camera->GetPosition();
        camPosition_y.y = m_billboardPos.y;
        vec3 fakeup = vec3(0, 1, 0);
        mat4 altViewMatrix = lookAt(camPosition_y, m_billboardPos, fakeup);

        glUniform3f(m_CameraRight_worldspace_ID, altViewMatrix[0][0], altViewMatrix[1][0], altViewMatrix[2][0]);
        glUniform3f(m_CameraUp_worldspace_ID,    altViewMatrix[0][1], altViewMatrix[1][1], altViewMatrix[2][1]);
    }


    // The billboard will be just above the cube
    // and 1m*12cm, because it matches its 256*32 resolution =)
    glUniform3f(m_billboardPosID,  m_billboardPos.x, m_billboardPos.y, m_billboardPos.z);
    glUniform2f(m_billboardSizeID, m_billboardScl.x, m_billboardScl.y);

                                                    // Generate some fake life level and send it to glsl
    float LifeLevel = 0.5f;// sin(currentTime)*0.1f + 0.7f;
    glUniform1f(t_LifeLevelID, LifeLevel);

    glUniformMatrix4fv(m_ViewProjMatrixID, 1, GL_FALSE, &ViewProjectionMatrix[0][0]);

    // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(0); // vertexbuffer 0.
    glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);

    // attribute. No particular reason for 0, but must match the layout in the shader.
    // size, type, normalized?, stride, array buffer offset
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0 );


    // Draw the billboard !
    // This draws a triangle_strip which looks like a quad.
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    //glDisableVertexAttribArray(0);

}

void Billboard::Cleanup() {
    // Cleanup VBO and shader
    glDeleteProgram(m_programID);
    glDeleteBuffers(1, &m_vertex_buffer);
    glDeleteTextures(1, &m_textureID);
}