#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "json.h"
using json = nlohmann::json;

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>

namespace engine
{
class Geometry
{
public:
    unsigned int VAO;
    unsigned int VBO;

    std::string name;
    std::vector<std::string> fieldNames;
    std::map<std::string, unsigned int> fieldWidths;
    std::vector<unsigned int> vertexIndices;
    unsigned int numVertices;

    Geometry(const char *filePath)
    {
        // read file
        json vertexJson;
        std::ifstream vertexFile(filePath);
        vertexFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            vertexJson = json::parse(vertexFile);
        }
        catch (json::exception& e)
        {
            std::cout << "ERROR::GEOMETRY::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
        }

        // parse vertex json
        this->name = vertexJson[std::string("name")].get<std::string>();
        this->vertexIndices = vertexJson[std::string("indices")].get< std::vector<unsigned int> >();
        this->fieldNames = vertexJson[std::string("field_names")].get< std::vector<std::string> >();
        this->fieldWidths = vertexJson[std::string("field_widths")].get< std::map<std::string, unsigned int> >();
        this->data = vertexJson[std::string("data")].get< std::vector<float> >();

        unsigned int stride = 0;
        for (auto it = this->fieldWidths.begin(); it != this->fieldWidths.end(); ++it)
        {
            stride += (unsigned int)it->second;
        }
        if (this->data.size() % stride != 0)
        {
            throw std::runtime_error("ERROR::GEOMETRY::INVALID_VERTEX_DATA");
        }
        this->numVertices = this->data.size() / stride;

        // generate VAO and VBO
        glGenVertexArrays(1, &(this->VAO));
        glBindVertexArray(this->VAO);

        glGenBuffers(1, &(this->VBO));
        glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
        glBufferData(GL_ARRAY_BUFFER, this->data.size() * sizeof(float), &(this->data[0]), GL_STATIC_DRAW);

        int attribID = 0;
        unsigned int offset = 0;
        for (auto it = this->fieldNames.begin(); it != this->fieldNames.end(); ++it)
        {
            unsigned fieldWidth = this->fieldWidths[*it];
            glVertexAttribPointer(
                attribID,
                fieldWidth,
                GL_FLOAT,
                GL_FALSE,
                stride * sizeof(float),
                (void *)(offset * sizeof(float))
            );
            glEnableVertexAttribArray(attribID);

            offset += fieldWidth;
            ++attribID;
        }

        glBindVertexArray(0);

        // TODO generate EBO
    }

    float *getDataArray(unsigned int &size)
    {
        size = this->data.size();
        return &(this->data[0]);
    }

private:
    std::vector<float> data;
};
}

#endif
