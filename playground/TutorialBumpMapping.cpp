#include "TutorialBumpMapping.h"

#include "ShaderReader.h"
#include "TextureLoader.h"
#include "ObjReader.h"

#include <common/controls.hpp>
#include <common/objloader.hpp>
#include <common/shader.hpp>
#include <common/texture.hpp>
#include "common/tangentspace.hpp"
#include "common/vboindexer.hpp"

void TutorialBumpMapping::Render_Loop_Setup(GLFWwindow* window) {
    m_freeCamera = new FreeCamera(window);
    vector<string> vs;

    ShaderReader  sr;
    ObjReader     objR;
    TextureLoader tl;

    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    programID = sr.LoadShaders("NormalMapping.vertexshader", "NormalMapping.fragmentshader");
    //programID = LoadShaders("NormalMapping.vertexshader", "NormalMapping.fragmentshader");

    // Get a handle for our "MVP" uniform
    MatrixID      = glGetUniformLocation(programID, "MVP");
    ModelMatrixID = glGetUniformLocation(programID, "M");
    ViewMatrixID  = glGetUniformLocation(programID, "V");
    ModelView3x3MatrixID = glGetUniformLocation(programID, "MV3x3");


    ///*
    vs = { "diffuse.DDS","specular.DDS"};
    vector<int> textures_loaded = tl.LoadFiles(vs);
    DiffuseTexture  = textures_loaded[0];
    SpecularTexture = textures_loaded[1];
    NormalTexture   = tl.LoadBmpAsNormalMap("normal.bmp");
    //*/
    DiffuseTextureID  = glGetUniformLocation(programID, "DiffuseTextureSampler");
    NormalTextureID   = glGetUniformLocation(programID, "NormalTextureSampler");
    SpecularTextureID = glGetUniformLocation(programID, "SpecularTextureSampler");


    vs = { "cylinder.obj" };
    objR.LoadFiles(vs);
    vector<vec3> vertices = objR.GetVertices();
    vector<vec3> normals  = objR.GetNormals();
    vector<vec2> uvs      = objR.GetUVs();

    vector<vec3> tangents;
    vector<vec3> bitangents;
    computeTangentBasis(
        vertices, uvs, normals, // input
        tangents, bitangents    // output
    );

    vector<vec2> indexed_uvs;
    
    indexVBO_TBN(vertices, uvs, normals, tangents, bitangents,
        m_indices, indexed_vertices, indexed_uvs, indexed_normals,
        indexed_tangents, indexed_bitangents);

    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(vec3), &indexed_vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_uvs.size() * sizeof(vec2), &indexed_uvs[0], GL_STATIC_DRAW);

    glGenBuffers(1, &normalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(vec3), &indexed_normals[0], GL_STATIC_DRAW);

    glGenBuffers(1, &tangentbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, tangentbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_tangents.size() * sizeof(vec3), &indexed_tangents[0], GL_STATIC_DRAW);

    glGenBuffers(1, &bitangentbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, bitangentbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_bitangents.size() * sizeof(vec3), &indexed_bitangents[0], GL_STATIC_DRAW);

    // Generate a buffer for the indices as well
    glGenBuffers(1, &elementbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned short), &m_indices[0], GL_STATIC_DRAW);

    // Get a handle for our "LightPosition" uniform
    glUseProgram(programID);
    LightID = glGetUniformLocation(programID, "LightPosition_worldspace");
}

void TutorialBumpMapping::Render_Loop_Implementation() {

    ////////////////////

    glUseProgram(programID);

    // Compute the MVP matrix from keyboard and mouse input
    m_freeCamera->Update();

    mat4 ProjectionMatrix = m_freeCamera->GetProjectionMatrix();
    mat4 ViewMatrix       = m_freeCamera->GetViewMatrix();
    mat4 ModelMatrix      = mat4(1.0);

    mat4 ModelViewMatrix    = ViewMatrix * ModelMatrix;
    mat3 ModelView3x3Matrix = mat3(ModelViewMatrix);
    mat4 MVP                = ProjectionMatrix * ViewMatrix * ModelMatrix;

    // Send our transformation to the currently bound shader, 
    // in the "MVP" uniform
    glUniformMatrix4fv(MatrixID,      1, GL_FALSE, &MVP[0][0]);
    glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
    glUniformMatrix4fv(ViewMatrixID,  1, GL_FALSE, &ViewMatrix[0][0]);
    glUniformMatrix4fv(ViewMatrixID,  1, GL_FALSE, &ViewMatrix[0][0]);
    glUniformMatrix3fv(ModelView3x3MatrixID, 1, GL_FALSE, &ModelView3x3Matrix[0][0]);


    vec3 lightPos = vec3(0, 0, 4);
    glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);

    // Bind our diffuse texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, DiffuseTexture);
    // Set our "DiffuseTextureSampler" sampler to user Texture Unit 0
    glUniform1i(DiffuseTextureID, 0);

    // Bind our normal texture in Texture Unit 1
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, NormalTexture);
    // Set our "Normal	TextureSampler" sampler to user Texture Unit 0
    glUniform1i(NormalTextureID, 1);

    // Bind our normal texture in Texture Unit 2
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, SpecularTexture);
    // Set our "Normal	TextureSampler" sampler to user Texture Unit 0
    glUniform1i(SpecularTextureID, 2);


    // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    // attribute, size, type, normalized?, stride, array buffer offset
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0 );

    // 2nd attribute buffer : UVs
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    // attribute, size, type, normalized?, stride, array buffer offset
    glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE,0, (void*)0 );

    // 3rd attribute buffer : normals
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    // attribute, size, type, normalized?, stride, array buffer offset
    glVertexAttribPointer( 2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0 );

    // 4th attribute buffer : tangents
    glEnableVertexAttribArray(3);
    glBindBuffer(GL_ARRAY_BUFFER, tangentbuffer);
    // attribute, size, type, normalized?, stride, array buffer offset
    glVertexAttribPointer( 3, 3, GL_FLOAT, GL_FALSE, 0, (void*)0 );

    // 5th attribute buffer : bitangents
    glEnableVertexAttribArray(4);
    glBindBuffer(GL_ARRAY_BUFFER, bitangentbuffer);
    // attribute, size, type, normalized?, stride, array buffer offset
    glVertexAttribPointer( 4, 3, GL_FLOAT, GL_FALSE, 0, (void*)0 );

    // Index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);

    // Draw the triangles !
    // mode, count, type, element array buffer offset
    glDrawElements( GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_SHORT, (void*)0 );

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);
    glDisableVertexAttribArray(4);


    ////////////////////////////////////////////////////////
    // DEBUG ONLY !!!
    // Don't use this in real code !!
    ////////////////////////////////////////////////////////


    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf((const GLfloat*)&ProjectionMatrix[0]);
    glMatrixMode(GL_MODELVIEW);
    glm::mat4 MV = ViewMatrix * ModelMatrix;
    glLoadMatrixf((const GLfloat*)&MV[0]);


    glUseProgram(0);

    // normals
    glColor3f(0, 0, 1);
    glBegin(GL_LINES);
    for (unsigned int i = 0; i < m_indices.size(); i++) {
        glm::vec3 p = indexed_vertices[m_indices[i]];
        glVertex3fv(&p.x);
        glm::vec3 o = glm::normalize(indexed_normals[m_indices[i]]);
        p += o*0.1f;
        glVertex3fv(&p.x);
    }
    glEnd();
    // tangents
    glColor3f(1, 0, 0);
    glBegin(GL_LINES);
    for (unsigned int i = 0; i<m_indices.size(); i++) {
        glm::vec3 p = indexed_vertices[m_indices[i]];
        glVertex3fv(&p.x);
        glm::vec3 o = glm::normalize(indexed_tangents[m_indices[i]]);
        p += o*0.1f;
        glVertex3fv(&p.x);
    }
    glEnd();
    // bitangents
    glColor3f(0, 1, 0);
    glBegin(GL_LINES);
    for (unsigned int i = 0; i<m_indices.size(); i++) {
        glm::vec3 p = indexed_vertices[m_indices[i]];
        glVertex3fv(&p.x);
        glm::vec3 o = glm::normalize(indexed_bitangents[m_indices[i]]);
        p += o*0.1f;
        glVertex3fv(&p.x);
    }
    glEnd();
    // light pos
    glColor3f(1, 1, 1);
    glBegin(GL_LINES);
    glVertex3fv(&lightPos.x);
    lightPos += glm::vec3(1, 0, 0)*0.1f;
    glVertex3fv(&lightPos.x);
    lightPos -= glm::vec3(1, 0, 0)*0.1f;
    glVertex3fv(&lightPos.x);
    lightPos += glm::vec3(0, 1, 0)*0.1f;
    glVertex3fv(&lightPos.x);
    glEnd();

}

void TutorialBumpMapping::Render_Loop_Cleanup() {
    // Cleanup VBO and shader
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteBuffers(1, &uvbuffer);
    glDeleteBuffers(1, &normalbuffer);
    glDeleteBuffers(1, &tangentbuffer);
    glDeleteBuffers(1, &bitangentbuffer);
    glDeleteBuffers(1, &elementbuffer);
    glDeleteProgram(programID);
    glDeleteTextures(1, &DiffuseTexture);
    glDeleteTextures(1, &NormalTexture);
    glDeleteTextures(1, &SpecularTexture);
    glDeleteVertexArrays(1, &VertexArrayID);
}