#ifndef INTERFACES_H
#define INTERFACES_H

#include <glm/glm.hpp>


struct Command
{
    int transRight = 0;

    void reset()
    {
        this->transRight = 0;
    }
};


#endif
