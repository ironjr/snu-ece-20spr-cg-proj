#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION   // use of stb functions once and for all
#include "stb_image.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>


namespace engine
{
class Texture
{
public:
    unsigned int ID;
    int width;
    int height;
    int channels;

    Texture(const char* filePath)
    {   
        glGenTextures(1, &(this->ID));
        glBindTexture(GL_TEXTURE_2D, this->ID);

        // Set the texture wrapping parameters
        // Set texture wrapping to GL_REPEAT (default wrapping method)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        // Set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_set_flip_vertically_on_load(true);

        unsigned char *data = stbi_load(filePath, &(this->width), &(this->height), &(this->channels), 0);
        if (data)
        {   
            if (channels == 3)
                glTexImage2D(
                    GL_TEXTURE_2D,
                    0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE,
                    data
                );
            else if (channels > 3)
                glTexImage2D(
                    GL_TEXTURE_2D,
                    0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                    data
                );
            else
                std::cout << "ERROR::TEXTURE::WRONG_CHANNEL_SIZE" << std::endl;
            
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            std::cout << "ERROR::TEXTURE::FAILED_TO_LOAD_TEXTURE" << std::endl;
        }

        stbi_image_free(data);
    }
};
}

#endif
