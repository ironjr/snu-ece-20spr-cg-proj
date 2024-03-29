#ifndef MATH_UTILS_H
#define MATH_UTILS_H

#include <iostream>
#include <vector>


struct AABB{
    float left;
    float right;
    float top;
    float bottom;
};

float getUniformRandomf()
{
    return (float)rand() / (float)RAND_MAX;
}

float getUniformRandomBetweenf(float a, float b)
{
    float r = getUniformRandomf();
    return a * (1 - r) + b * r;
}

glm::quat getRandomQuaternion()
{
    float angle = glm::mod(getUniformRandomBetweenf(0.0f, 360.0f), 360.0f);
    glm::vec3 axis = glm::normalize(glm::vec3(
        getUniformRandomf(),
        getUniformRandomf(),
        getUniformRandomf()
    ));
    return glm::angleAxis(glm::radians(angle), axis);
}

glm::mat4 getRandomRotation()
{
    return glm::mat4_cast(getRandomQuaternion());
}

#endif
