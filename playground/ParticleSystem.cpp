#include "ParticleSystem.h"

// stl
#include <algorithm>

#include <glfw3.h>
#include <glm/gtx/norm.hpp>

#include "ShaderReader.h"
#include "TextureLoader.h"


using namespace std;

ParticleSystem::ParticleSystem() {
    m_lastUsedParticle = 0;
    m_particlesCount   = 0;
}

void ParticleSystem::Initialize(ICamera* camera) {
    TextureLoader tl;
    ShaderReader sr;

    p_camera = camera;

    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    programID = sr.LoadShaders("Particle.vertexshader", "Particle.fragmentshader");

    // Vertex shader
    CameraRight_worldspace_ID = glGetUniformLocation(programID, "CameraRight_worldspace");
    CameraUp_worldspace_ID    = glGetUniformLocation(programID, "CameraUp_worldspace");
    ViewProjMatrixID          = glGetUniformLocation(programID, "VP");

    // fragment shader
    TextureID = glGetUniformLocation(programID, "myTextureSampler");

    m_particule_position_size_data = new GLfloat[MAX_PARTICLES * 4];
    m_particule_color_data         = new GLubyte[MAX_PARTICLES * 4];
    m_ParticlesContainer           = new Particle[MAX_PARTICLES];

    for (int i = 0; i < MAX_PARTICLES; i++) {
        m_ParticlesContainer[i].life = -1.0f;
        m_ParticlesContainer[i].cameradistance = -1.0f;
    }

    vector<string> vs = { "particle.DDS" };
    Texture = tl.LoadFiles(vs)[0];

    glGenBuffers(1, &billboard_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(PARTICLE_STRUCTURE), PARTICLE_STRUCTURE, GL_STATIC_DRAW);

    // The VBO containing the positions and sizes of the particles
    glGenBuffers(1, &particles_position_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
    // Initialize with empty (NULL) buffer : it will be updated later, each frame.
    glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);

    // The VBO containing the colors of the particles
    glGenBuffers(1, &particles_color_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
    // Initialize with empty (NULL) buffer : it will be updated later, each frame.
    glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);

    m_lastTime = glfwGetTime();

}

void ParticleSystem::Render() {
    mat4 ProjectionMatrix = p_camera->GetProjectionMatrix();
    mat4 ViewMatrix       = p_camera->GetViewMatrix();

    mat4 ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;

    // Update the buffers that OpenGL uses for rendering.
    // There are much more sophisticated means to stream data from the CPU to the GPU, 
    // but this is outside the scope of this tutorial.
    // http://www.opengl.org/wiki/Buffer_Object_Streaming
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Use our shader
    glUseProgram(programID);

    glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
    glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
    glBufferSubData(GL_ARRAY_BUFFER, 0, m_particlesCount * sizeof(GLfloat) * 4, m_particule_position_size_data);

    glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
    glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
    glBufferSubData(GL_ARRAY_BUFFER, 0, m_particlesCount * sizeof(GLubyte) * 4, m_particule_color_data);


    // Bind our texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Texture);
    // Set our "myTextureSampler" sampler to user Texture Unit 0
    glUniform1i(TextureID, 0);

    // Same as the billboards tutorial
    glUniform3f(CameraRight_worldspace_ID, ViewMatrix[0][0], ViewMatrix[1][0], ViewMatrix[2][0]);
    glUniform3f(CameraUp_worldspace_ID, ViewMatrix[0][1], ViewMatrix[1][1], ViewMatrix[2][1]);

    glUniformMatrix4fv(ViewProjMatrixID, 1, GL_FALSE, &ViewProjectionMatrix[0][0]);


    // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
    // attribute. No particular reason for 0, but must match the layout in the shader.
    // size, type, normalized?, stride, array buffer offset
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // 2nd attribute buffer : positions of particles' centers
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
    // attribute. No particular reason for 1, but must match the layout in the shader.
    // size : x + y + z + size => 4, type, normalized?, stride, array buffer offset
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

    ///*
    // 3rd attribute buffer : particles' colors
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
    // attribute. No particular reason for 2, but must match the layout in the shader.
    // size : r + g + b + a => 4, type
    // normalized?    *** YES, this means that the unsigned char[4] will be accessible with a vec4 (floats) in the shader ***
    // stride, array buffer offset
    glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, (void*)0);

    //*/

    // These functions are specific to glDrawArrays*Instanced*.
    // The first parameter is the attribute buffer we're talking about.
    // The second parameter is the "rate at which generic vertex attributes advance when rendering multiple instances"
    // http://www.opengl.org/sdk/docs/man/xhtml/glVertexAttribDivisor.xml
    glVertexAttribDivisor(0, 0); // particles vertices : always reuse the same 4 vertices -> 0
    glVertexAttribDivisor(1, 1); // positions : one per quad (its center)                 -> 1
    glVertexAttribDivisor(2, 1); // color : one per quad                                  -> 1

                                 // Draw the particules !
                                 // This draws many times a small triangle_strip (which looks like a quad).
                                 // This is equivalent to :
                                 // for(i in ParticlesCount) : glDrawArrays(GL_TRIANGLE_STRIP, 0, 4), 
                                 // but faster.
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, m_particlesCount);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

    // make sure to "close" the "rate at which generic vertex attributes advance when rendering multiple instances"
    // otherwise we see leaking to other calls.
    glVertexAttribDivisor(0, 0);
    glVertexAttribDivisor(1, 0);
    glVertexAttribDivisor(2, 0);
}

void ParticleSystem::Update() {

    mat4 ViewMatrix = p_camera->GetViewMatrix();
    // We will need the camera's position in order to sort the particles
    // w.r.t the camera's distance.
    // There should be a getCameraPosition() function in common/controls.cpp, 
    // but this works too.
    vec3 CameraPosition(inverse(ViewMatrix)[3]);

    double currentTime = glfwGetTime();
    double delta       = currentTime - m_lastTime;
    m_lastTime = currentTime;

    // Generate 10 new particule each millisecond,
    // but limit this to 16 ms (60 fps), or if you have 1 long frame (1sec),
    // newparticles will be huge and the next frame even longer.
    int newparticles = (int)(delta*10000.0);
    if (newparticles > (int)(0.016f*10000.0)) {
        newparticles = (int)(0.016f*10000.0);
    }

    for (int i = 0; i<newparticles; i++) {
        int particleIndex = FindUnusedParticle();
        m_ParticlesContainer[particleIndex].life = 5.0f; // This particle will live 5 seconds.
        m_ParticlesContainer[particleIndex].pos = glm::vec3(0, 0, -20.0f);

        float spread = 1.5f;
        glm::vec3 maindir = glm::vec3(0.0f, 10.0f, 0.0f);
        // Very bad way to generate a random direction; 
        // See for instance http://stackoverflow.com/questions/5408276/python-uniform-spherical-distribution instead,
        // combined with some user-controlled parameters (main direction, spread, etc)
        vec3 randomdir = vec3((rand() % 2000 - 1000.0f) / 1000.0f,
                              (rand() % 2000 - 1000.0f) / 1000.0f,
                              (rand() % 2000 - 1000.0f) / 1000.0f );

        m_ParticlesContainer[particleIndex].speed = maindir + randomdir*spread;


        // Very bad way to generate a random color
        m_ParticlesContainer[particleIndex].r = rand() % 256;
        m_ParticlesContainer[particleIndex].g = rand() % 256;
        m_ParticlesContainer[particleIndex].b = rand() % 256;
        m_ParticlesContainer[particleIndex].a = (rand() % 256) / 3;

        m_ParticlesContainer[particleIndex].size = (rand() % 1000) / 2000.0f + 0.1f;

    }

    // Simulate all particles
    m_particlesCount = 0;
    for (int i = 0; i<MAX_PARTICLES; i++) {
        Particle& p = m_ParticlesContainer[i]; // shortcut
        if (p.life > 0.0f) {
            // Decrease life
            p.life -= delta;
            if (p.life > 0.0f) {
                // Simulate simple physics : gravity only, no collisions
                p.speed += vec3(0.0f, -9.81f, 0.0f) * (float)delta * 0.5f;
                p.pos   += p.speed * (float)delta;
                p.cameradistance = length2(p.pos - CameraPosition);
                //ParticlesContainer[i].pos += glm::vec3(0.0f,10.0f, 0.0f) * (float)delta;

                // Fill the GPU buffer
                m_particule_position_size_data[4 * m_particlesCount + 0] = p.pos.x;
                m_particule_position_size_data[4 * m_particlesCount + 1] = p.pos.y;
                m_particule_position_size_data[4 * m_particlesCount + 2] = p.pos.z;

                m_particule_position_size_data[4 * m_particlesCount + 3] = p.size;

                m_particule_color_data[4 * m_particlesCount + 0] = p.r;
                m_particule_color_data[4 * m_particlesCount + 1] = p.g;
                m_particule_color_data[4 * m_particlesCount + 2] = p.b;
                m_particule_color_data[4 * m_particlesCount + 3] = p.a;

            }
            else {
                // Particles that just died will be put at the end of the buffer in SortParticles();
                p.cameradistance = -1.0f;
            }
            m_particlesCount++;
        }
    }
    SortParticles();
}

void ParticleSystem::Cleanup() {
    delete[] m_particule_position_size_data;
    delete[] m_particule_color_data;

    // Cleanup VBO and shader
    glDeleteBuffers(1, &particles_color_buffer);
    glDeleteBuffers(1, &particles_position_buffer);
    glDeleteBuffers(1, &billboard_vertex_buffer);
    glDeleteProgram(programID);
    glDeleteTextures(1, &TextureID);
    //glDeleteVertexArrays(1, &VertexArrayID);

    glDeleteVertexArrays(1, &VertexArrayID);

}

int ParticleSystem::FindUnusedParticle() {

    for (int i = m_lastUsedParticle; i<MAX_PARTICLES; i++) {
        if (m_ParticlesContainer[i].life < 0) {
            m_lastUsedParticle = i;
            return i;
        }
    }

    for (int i = 0; i<m_lastUsedParticle; i++) {
        if (m_ParticlesContainer[i].life < 0) {
            m_lastUsedParticle = i;
            return i;
        }
    }

    return 0; // All particles are taken, override the first one
}

void ParticleSystem::SortParticles() {
    sort(&m_ParticlesContainer[0], &m_ParticlesContainer[MAX_PARTICLES]);
}