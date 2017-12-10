#include "ParticleSystem.h"

// Returns a random float between 0 and the passed in argument
float randNumber(float max)
{
    return ((float)rand() / (float)RAND_MAX) * max;
}

// Returns a random float between two arguments
float randBWtwo(float a, float b)
{
    return randNumber(b - a) + a;
}

void ParticleSystem::draw(glm::vec3 lookAtPoint, glm::vec3 suzPoint, Vertex points[], GLint bbsize, GLint particleHandle, GLuint pointsVAO, GLuint pointsVBO)
{
    // The following sorts particles for billboarding and calls the individual draw functions
    glm::vec3 viewVec = lookAtPoint - suzPoint;
    glm::normalize(viewVec);
    glm::mat4 modelMtx;

    int orderedIndices[particles.size()];
    double distances[particles.size()];

    for (unsigned int i = 0; i < particles.size(); i++)
    {
        Vertex point = particles.at(i).position;
        glm::vec4 p = glm::vec4(point.x, point.y, point.z, 1);
        p = p * modelMtx;
        glm::vec4 ep = p - glm::vec4(suzPoint, 1);

        orderedIndices[i] = i;
        distances[i] = glm::dot(glm::vec4(viewVec, 0), ep);
    }

    for (unsigned int i = 0; i < particles.size(); i++)
    {
        for (unsigned int j = 0; j < particles.size(); j++)
        {
            if (distances[j] <= distances[i])
            {
                double temp = distances[i];
                int tempLoc = orderedIndices[i];

                orderedIndices[i] = orderedIndices[j];
                orderedIndices[j] = tempLoc;

                distances[i] = distances[j];
                distances[j] = temp;
            }
        }
    }

    std::vector<Particle> orderedPoints;

    for (unsigned int i = 0; i < particles.size(); i++)
    {
        orderedPoints.push_back(particles.at(orderedIndices[i]));
    }

    for (unsigned int i = 0; i < orderedPoints.size(); i++)
    {
        orderedPoints.at(i).draw(points, bbsize, particleHandle, pointsVAO, pointsVBO);
    }
}

void ParticleSystem::update()
{
    // Updating the position of every particle in the system
    std::vector<Particle> newparticles;
    for (unsigned int i = 0; i < particles.size(); i++)
    {
        particles.at(i).age += 0.01f;
        particles.at(i).update();

        if (particles.at(i).age < particles.at(i).maxAge)
        {
            newparticles.push_back(particles.at(i));
        }
    }
    particles = newparticles;
}

void ParticleSystem::add()
{
    // Creating a new point and adding it to the current system's particle vector
    Particle newParticle;
    newParticle.position = startingLoc;
    newParticle.velocity = randBWtwo(minV, maxV);
    newParticle.maxAge = randBWtwo(minLife, maxLife);
    newParticle.phi = randNumber(coneAngle) * 3.14f / 180.0f;
    newParticle.theta = randNumber(360.0f) * 3.14f / 180.0f;
    newParticle.emitter = startingLoc;
    newParticle.age = 0.0f;
    newParticle.genDirection();
    particles.push_back(newParticle);
}