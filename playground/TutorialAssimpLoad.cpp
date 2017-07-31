#include "TutorialAssimpLoad.h"

#include <glm/gtc/matrix_transform.hpp>

#include "TextureLoader.h"
#include "ShaderReader.h"


void TutorialAssimpLoad::Render_Loop_Setup(GLFWwindow* window) {

    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    m_freeCamera = new FreeCamera(window);

    vector<string> vs;
    ShaderReader sr;
    vs = { "StandardShading.vertexshader", "StandardShading.fragmentshader" };
    programID = sr.LoadFiles(vs)[0];
    MatrixID      = glGetUniformLocation(programID, "MVP");
    ModelMatrixID = glGetUniformLocation(programID, "M");
    ViewMatrixID  = glGetUniformLocation(programID, "V");

    vs = { "boblampclean.md5mesh" }; //"duck.dae" };
    m_assimpW = new AssimpWrapper();
    m_assimpW->LoadFiles(vs);
    m_assimpW->FinalizeVBO();

    TextureLoader tl;
    vs = { "uvmap_monkey.DDS" };
    Texture   = tl.LoadFiles(vs)[0];
    TextureID = glGetUniformLocation(programID, "myTextureSampler");

    LightID = glGetUniformLocation(programID, "LightPosition_worldspace");
}

void TutorialAssimpLoad::Render_Loop_Implementation() {
    glUseProgram(programID);

    m_freeCamera->Update();
    mat4 ProjectionMatrix = m_freeCamera->GetProjectionMatrix();
    mat4 ViewMatrix = m_freeCamera->GetViewMatrix();
    mat4 ModelMatrix = mat4(1.0f);
    vec3 vScale = vec3(.01f, .01f, .01f);
    ModelMatrix = glm::scale(ModelMatrix, vScale);
    mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

    vec3 lightPos = vec3(-4, 4, 4);
    glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);

    glUniformMatrix4fv(ViewMatrixID,  1, GL_FALSE, &ViewMatrix[0][0]);
    glUniformMatrix4fv(MatrixID,      1, GL_FALSE, &MVP[0][0]);
    glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);

    m_assimpW->Render();

}

void TutorialAssimpLoad::Render_Loop_Cleanup() {
    delete m_assimpW;
    m_assimpW = 0;
}