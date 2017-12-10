#pragma once
#include <math.h>
#include <GL/glew.h>    // include GLEW to get our OpenGL 3.0+ bindings
#include <glm/glm.hpp>
#include <iostream>

struct Vertex
{
    float x, y, z;
};

class Particle
{
  public:
    float maxAge;
    float age;
    float velocity;
    float phi;
    float theta;
    float size;
    Vertex position;
    Vertex emitter;
    Vertex direction;

    void genDirection();

    void update();

    void draw(Vertex points[], GLint bbsize, GLint particleHandle, GLuint pointsVAO, GLuint pointsVBO);
};