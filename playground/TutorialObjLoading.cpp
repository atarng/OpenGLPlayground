#include "TutorialObjLoading.h"


#include <GL/glew.h>   // Include GLEW
#include <glfw3.h>     // Include GLFW
#include <glm/gtc/matrix_transform.hpp>


#include <common/vboindexer.hpp>

#include "ShaderReader.h"
#include "ObjReader.h"
#include "TextureLoader.h"



void TutorialObjLoading::Render_Loop_Setup(GLFWwindow* window) {
    /******* Start Tutorial 2A *******/
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    vector<string> vs;


    m_freeCamera = new FreeCamera(window);

    m_particles  = new ParticleSystem();
    m_particles->Initialize(m_freeCamera);

    m_billboard  = new Billboard[2];
    m_billboard[0].Initialize(m_freeCamera, false, false);
    m_billboard[1].Initialize(m_freeCamera, true);
    m_billboard[1].SetPosition(vec3{ 3.0f, 1.25f, 0.0f });
    

    // Create and compile our GLSL program from the shaders
    //GLuint programID = LoadShaders("SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader");
    //GLuint programID = LoadShaders("StandardShading.vertexshader", "StandardShading.fragmentshader");
    ShaderReader sr;
    vs = { "StandardShading.vertexshader", "StandardShading.fragmentshader" };
    programID = sr.LoadFiles(vs)[0];

    vs = { "StandardShading.vertexshader", "StandardTransparentShading.fragmentshader" };
    transProgramID = sr.LoadFiles(vs)[0];

    MatrixID      = glGetUniformLocation(programID, "MVP");
    ModelMatrixID = glGetUniformLocation(programID, "M");
    ViewMatrixID  = glGetUniformLocation(programID, "V");

    /****
    MatrixID2       = glGetUniformLocation(transProgramID, "MVP");
    ModelMatrixID2  = glGetUniformLocation(transProgramID, "M");
    ViewMatrixID2   = glGetUniformLocation(transProgramID, "V");
    ****/

    TextureLoader tl;
    vs = { "uvmap_monkey.DDS" };
    Texture = tl.LoadFiles(vs)[0]; //loadDDS("uvmap.DDS");
    TextureID = glGetUniformLocation(programID, "myTextureSampler");

    /***
    TextureID2 = glGetUniformLocation(transProgramID, "myTextureSampler");
    ***/

    vs = { "Holstein.DDS" };
    m_fontLoader = new Font2dLoader();
    m_fontLoader->LoadFiles(vs);


    vs = { "suzanne.obj" };
    ObjReader objR;
    objR.LoadFiles(vs);
    vector<vec2> uvs      = objR.GetUVs();
    vector<vec3> normals  = objR.GetNormals();
    m_vertices = objR.GetVertices();


    vector<vec2> indexed_uvs;
    vector<vec3> indexed_normals, indexed_vertices;
    indexVBO(m_vertices, uvs, normals, m_indices, indexed_vertices, indexed_uvs, indexed_normals);

    /**************************/
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(vec3), &indexed_vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_uvs.size() * sizeof(vec2), &indexed_uvs[0], GL_STATIC_DRAW);

    glGenBuffers(1, &normalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(vec3), &indexed_normals[0], GL_STATIC_DRAW);

    glGenBuffers(1, &elementbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned short), &m_indices[0], GL_STATIC_DRAW);
    /**************************/

    /*** Original, non-indexed vbo *******/
    glGenBuffers(1, &vertexbuffer2);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer2);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(vec3), &m_vertices[0], GL_STATIC_DRAW);
    glGenBuffers(1, &uvbuffer2);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer2);
    glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(vec2), &uvs[0], GL_STATIC_DRAW);
    glGenBuffers(1, &normalbuffer2);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer2);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(vec3), &normals[0], GL_STATIC_DRAW);
    /**************************************/

    // Get a handle for our "LightPosition" uniform
    glUseProgram(programID);
    LightID = glGetUniformLocation(programID, "LightPosition_worldspace");

    //glUseProgram(transProgramID);
    //LightID2 = glGetUniformLocation(transProgramID, "LightPosition_worldspace");

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void TutorialObjLoading::Render_Loop_Implementation() {

    /*** Independent of seperate objects ***/
    m_freeCamera->Update();

    m_particles->Update();
    m_particles->Render();

    m_billboard[0].Render();
    m_billboard[1].Render();


    mat4 ProjectionMatrix = m_freeCamera->GetProjectionMatrix();
    mat4 ViewMatrix       = m_freeCamera->GetViewMatrix();
    mat4 ModelMatrix = mat4(1.0);
    mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

    glDisable(GL_BLEND);

    // Start Tutorial 2B

    // Use our shader
    glUseProgram(programID);

    vec3 lightPos = vec3(-4, 4, 4);
    glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);

    glUniformMatrix4fv(ViewMatrixID,  1, GL_FALSE, &ViewMatrix[0][0]);
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
    glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);

    // Bind our texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Texture);
    // Set our "myTextureSampler" sampler to user Texture Unit 0
    glUniform1i(TextureID, 0);

    // 1st attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(0, 3,GL_FLOAT, GL_FALSE,0, (void*)0 ); // attribute, size, type,  normalized?, stride, array buffer offset

    // 2nd attribute buffer : UVs
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    // attribute, size, type,  normalized?, stride, array buffer offset
    glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0 );

    // 3rd attribute buffer : normals
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    // attribute, size, type, normalized?, stride, array buffer offset
    glVertexAttribPointer( 2,3,GL_FLOAT, GL_FALSE, 0, (void*)0 );


    // THIS REPLACES DRAW ARRAYS CALL :
    // Index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    // Draw the triangles !
    // mode, count, type, element array buffer offset
    glDrawElements( GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_SHORT, (void*)0 );

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

//// SECOND MONKEY /////////////////////////////////
// Tut10: Transparency
    glEnable(GL_BLEND);
    // because we are using the same vertexshaders the ids can be the same
    glUseProgram(transProgramID);

    //glUniform1i(TextureID2, 0);
    lightPos = vec3(0, 4, 0);
    // probably can get away with using same lightid because they are using the same vertex shader.
    glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z); 
    // !!!: must register this because of new shader.
    glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);

// BUT the Model matrix is different (and the MVP too)
    mat4 ModelMatrix2 = mat4(1.0);
    ModelMatrix2      = translate(ModelMatrix2, vec3(3.0f, 0.0f, 0.0f));
    mat4 MVP2         = ProjectionMatrix * ViewMatrix * ModelMatrix2;
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP2[0][0]);              //
    glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix2[0][0]); //

    // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer2);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    // 2nd attribute buffer : UVs
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer2);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    // 3rd attribute buffer : normals
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    // Draw the triangle !
    glDrawArrays(GL_TRIANGLES, 0, m_vertices.size());
//////////////////////////////////////

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);


    char text[256];
    sprintf(text, "%.2f sec", glfwGetTime());
    m_fontLoader->printText2D(text, 10, 500, 60);


}

void TutorialObjLoading::Render_Loop_Cleanup() {
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteBuffers(1, &uvbuffer);

    glDeleteBuffers(1, &normalbuffer);
    glDeleteBuffers(1, &elementbuffer);

    glDeleteProgram(programID);
    glDeleteProgram(transProgramID);

    glDeleteTextures(1, &Texture);
    glDeleteVertexArrays(1, &VertexArrayID);

    m_fontLoader->cleanupText2D();
    delete m_fontLoader;
    m_fontLoader = 0;

    delete m_freeCamera;
    m_freeCamera = 0;

    m_billboard[0].Cleanup();
    m_billboard[1].Cleanup();

}