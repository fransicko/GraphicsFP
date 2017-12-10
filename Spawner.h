#pragma once
#include "Marble.h"
#include <iostream>
#include <cstdlib>
#include <vector>
#include <math.h>
#include <glm/gtx/vector_angle.hpp>

class Spawner
{
  public:
    std::vector<Marble*> enemies;
    glm::vec3 startingLoc;
    glm::vec3 startingDir;
    float size;
    float spawn;
    double start;
    double updateStart;

    void draw(glm::mat4 modelMtx, glm::mat4 viewMtx, GLint uniform_modelMtx_loc, GLint uniform_color_loc, GLint uniform_normalMtx_loc);

    void update(glm::vec3 playerLoc);

    void add();
};