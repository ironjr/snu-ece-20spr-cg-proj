#ifndef GAMEOBJECTS_H
#define GAMEOBJECTS_H

#include <glm/glm.hpp>
#include "math_utils.h"


#define BAR_YPOS        -0.65f
#define BAR_LENGTH       0.3f
#define BAR_WIDTH        0.04f
#define BAR_SPEED        1.3f


class GameObject
{
public:
    glm::vec3 scale;
    glm::vec4 rotation;
    glm::vec3 position;
};

struct TriangleEntity
{
    float scale;
    float rotation;
    float dropSpeed;
    float rotationSpeed;
    glm::vec3 position;
    bool toDelete = false;
};

struct Bar
{   
    glm::vec3 scale;
    glm::vec3 position;
    float speed;

    Bar() : speed(BAR_SPEED)
    {
        scale.x = BAR_LENGTH;
        scale.y = BAR_WIDTH;
        scale.z = 1.0f;

        position.x = 0.0f;
        position.y = BAR_YPOS;
        position.z = 0.0f;
    }
};

TriangleEntity getRandomEntity()
{
    TriangleEntity e;
    e.scale = getRandomValueBetween(0.04f, 0.1f);
    e.rotation = getRandomValueBetween(0.1f, 30.0f);
    e.dropSpeed = getRandomValueBetween(0.1f, 0.4f);
    e.rotationSpeed = getRandomValueBetween(0.1f, 0.4f);
    e.position = glm::vec3(getRandomValueBetween(-0.9f, 0.9f), 1.0f, 0.0f);
    return e;
}

#endif
