#include "Particle.h"

void Particle::genDirection()
{
    // This sets up the trajectory of the particle
    direction.x = sin(theta) * sin(phi);
    direction.y = cos(phi);
    direction.z = cos(theta) * sin(phi);
}

void Particle::update()
{
    // Updating particle's position
    glm::vec3 d = glm::vec3(direction.x, direction.y, direction.z);
    d = normalize(d);
    position.x = emitter.x + velocity * d.x * age;
    position.y = emitter.y + velocity * d.y * age - 9.8 * age * age / 2.0f;
    position.z = emitter.z + velocity * d.z * age;
}

void Particle::draw(Vertex points[], GLint bbsize, GLint particleHandle, GLuint pointsVAO, GLuint pointsVBO)
{
    // Passing array is actually passing a pointer
    // That messes up sizeof()
    // This temp array of size 1 needed to get correct sizeof()
    Vertex temp[1];

    points[0] = position;
    glUniform1f(bbsize, 1.0f - age / maxAge);
    glBindVertexArray(pointsVAO);

    glBindBuffer(GL_ARRAY_BUFFER, pointsVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(temp), points);

    glBindTexture(GL_TEXTURE_2D, particleHandle);
    glDrawArrays(GL_POINTS, 0, 1);
}