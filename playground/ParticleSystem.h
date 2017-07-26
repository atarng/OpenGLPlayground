#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>

#include "ICamera.h"

using namespace atrng;
using namespace glm;

class ParticleSystem {
public:
    // CPU representation of a particle
    struct Particle {
        vec3 pos, speed;
        unsigned char r, g, b, a; // Color
        float size, angle, weight;
        float life; // Remaining life of the particle. if <0 : dead and unused.
        float cameradistance; // *Squared* distance to the camera. if dead : -1.0f

        bool operator<(const Particle& that) const {
            // Sort in reverse order : far particles drawn first.
            return this->cameradistance > that.cameradistance;
        }
    };

    ParticleSystem();

    void Initialize(ICamera*);
    void Update();
    void Render();
    void Cleanup();

    int FindUnusedParticle();
    void SortParticles();

protected:
private:
    const GLfloat PARTICLE_STRUCTURE[12] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
        -0.5f,  0.5f, 0.0f,
         0.5f,  0.5f, 0.0f,
    };
    double m_lastTime;

    const int MAX_PARTICLES = 100000;

    int m_lastUsedParticle;
    int m_particlesCount;
    Particle* m_ParticlesContainer;
    ICamera*  p_camera;

    GLuint programID, CameraRight_worldspace_ID, CameraUp_worldspace_ID, 
      VertexArrayID,
      ViewProjMatrixID, TextureID, Texture,
      billboard_vertex_buffer, particles_position_buffer, particles_color_buffer;


    ///
    GLfloat* m_particule_position_size_data;
    GLubyte* m_particule_color_data;
};