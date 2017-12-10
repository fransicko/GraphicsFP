#pragma once
#include "Particle.h"
#include <cstdlib>
#include <vector>

class ParticleSystem
{
  public:
    std::vector<Particle> particles;
    Vertex startingLoc;
    float minLife;
    float maxLife;
    float spawn;
    float minV;
    float maxV;
    float coneAngle;
    double start;
    double updateStart;

    void draw(glm::vec3 lookAtPoint, glm::vec3 suzPoint, Vertex points[], GLint bbsize, GLint particleHandle, GLuint pointsVAO, GLuint pointsVBO);

    void update();

    void add();
};