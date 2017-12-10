#include "Spawner.h"

bool checkOutOfBounds(Marble *mar)
{
    glm::vec3 loc = mar->location;
    double r = mar->radius;
    if (loc.x + r / 2 > 12 || loc.x < -12 - r / 2 || loc.z < -12 - r / 2)
    {
        return true;
    }
    return false;
}

void Spawner::draw(glm::mat4 modelMtx, GLint uniform_modelMtx_loc, GLint uniform_color_loc)
{
    for (unsigned int i = 0; i < enemies.size(); i++)
    {
        enemies.at(i)->draw(modelMtx, uniform_modelMtx_loc, uniform_color_loc);
    }
}

void Spawner::update(glm::vec3 playerLoc)
{
    for (unsigned int i = 0; i < enemies.size(); i++)
    {
        Marble *mar = enemies.at(i);
        if (checkOutOfBounds(mar))
        {
            enemies.erase(enemies.begin() + i);
            i--;
        }
        else
        {
            /*
            glm::vec3 newDir = glm::normalize(playerLoc - mar->location);
            float angleBW = acos(glm::dot(glm::normalize(mar->direction), newDir)) * 180.0f / 3.14f;
            float cap = 1.0f;
            if (angleBW < cap || std::isnan(angleBW))
            {
                mar->direction = newDir;
            }
            else
            {
                // Rotating enemy by a cap, for arcing motion
                mar->direction = glm::vec3(sin(cap * 3.14f / 180.0f) * mar->direction.z + cos(cap * 3.14f / 180.0f) * mar->direction.x,
                                           0,
                                           cos(cap * 3.14f / 180.0f) * mar->direction.z - sin(cap * 3.14f / 180.0f) * mar->direction.x);

                // Ensuring rotation of enemy is always reducing the angle differences
                float newangleBW = acos(glm::dot(glm::normalize(mar->direction), newDir)) * 180.0f / 3.14f;
                if (newangleBW > angleBW)
                {
                    cap *= -1;
                    mar->direction = glm::vec3(sin(2 * cap * 3.14f / 180.0f) * mar->direction.z + cos(2 * cap * 3.14f / 180.0f) * mar->direction.x,
                                               0,
                                               cos(2 * cap * 3.14f / 180.0f) * mar->direction.z - sin(2 * cap * 3.14f / 180.0f) * mar->direction.x);
                }
            } */
            mar->moveForward();
        }
    }
}

void Spawner::add()
{
    Marble *mar = new Marble(startingLoc, startingDir, size);
    enemies.push_back(mar);
}