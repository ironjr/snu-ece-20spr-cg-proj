#ifndef INTERFACE_H
#define INTERFACE_H


namespace engine
{
struct Commands
{
    int moveForward = 0;
    int moveRight = 0;

    void reset()
    {
        this->moveForward = 0;
        this->moveRight = 0;
    }
};
}

#endif
