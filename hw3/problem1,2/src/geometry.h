#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <vector>

#include "resource_utils.h"


namespace engine
{
enum class GeometryDataMode
{
    LINE,
    SURFACE
};


class Geometry
{
public:
    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;
    bool hasEBO;

    std::string name;
    std::vector<std::string> fieldNames;
    std::map<std::string, GLsizei> fieldWidths;
    GLsizei numVertices;
    GLsizei numVertexDataEntries;
    GLsizei recordSize;

    Geometry(const char *jsonPath)
    {
        // Read file.
        json vertexJson;
        readJson(jsonPath, vertexJson);

        // Parse vertex json.
        this->name = vertexJson[std::string("name")].get<std::string>();
        this->fieldNames = vertexJson[std::string("field_names")].get< std::vector<std::string> >();
        this->fieldWidths = vertexJson[std::string("field_widths")].get< std::map<std::string, GLsizei> >();
        this->vertexData = vertexJson[std::string("data")].get< std::vector<float> >();
        this->indexData = vertexJson[std::string("indices")].get< std::vector<unsigned int> >();
        this->hasEBO = this->indexData.size() != 0;

        this->initialize();
    }

    Geometry(const char *name, const char *filePath, GeometryDataMode mode)
    {
        this->name = std::string(name);

        switch (mode)
        {
            case GeometryDataMode::LINE:
                loadSplineControlPoints(std::string(filePath), this->numVertices, this->vertexData);
                this->fieldNames.push_back(std::string("position"));
                this->fieldWidths["position"] = (GLsizei)3;
                this->hasEBO = false;

                this->initialize();
                break;
            case GeometryDataMode::SURFACE:
                // TODO
                loadBezierSurfaceControlPoints(std::string(filePath));
                this->fieldNames.push_back(std::string("position"));
                this->fieldWidths["position"] = (GLsizei)3;
                this->hasEBO = false;

                this->initialize();
                break;
            default:
                throw std::invalid_argument("ERROR::GEOMETRY::WRONG_FILE_MODE");
        }
    }

    ~Geometry()
    {
        glDeleteVertexArrays(1, &(this->VAO));
        glDeleteBuffers(1, &(this->VBO));
        if (this->hasEBO)
            glDeleteBuffers(1, &(this->EBO));
    }

    float *getVertexDataArray(GLsizei &size)
    {
        size = (GLsizei)(this->vertexData.size());
        return &(this->vertexData[0]);
    }

    unsigned int *getIndexDataArray(GLsizei &size)
    {
        size = (GLsizei)(this->indexData.size());
        return &(this->indexData[0]);
    }

    GLsizei getSize() { return this->recordSize * this->numVertexDataEntries; }

private:
    std::vector<unsigned int> indexData;
    std::vector<float> vertexData;

    void initialize()
    {
        GLsizei stride = 0;
        for (auto const &elem : this->fieldWidths)
        {
            stride += (GLsizei)elem.second;
        }
        if (this->vertexData.size() % stride != 0)
        {
            throw std::runtime_error("ERROR::GEOMETRY::INVALID_VERTEX_DATA");
        }
        this->numVertexDataEntries = (GLsizei)(this->vertexData.size()) / stride;
		this->numVertices = this->hasEBO
            ? (GLsizei)this->indexData.size()
            : this->numVertexDataEntries;
        this->recordSize = stride;

        // Generate VAO and VBO.
        glGenVertexArrays(1, &(this->VAO));
        glBindVertexArray(this->VAO);

        glGenBuffers(1, &(this->VBO));
        glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
        glBufferData(
            GL_ARRAY_BUFFER,
            this->vertexData.size() * sizeof(float),
            &(this->vertexData[0]),
            GL_STATIC_DRAW
        );

        // Generate EBO if index data exists.
        if (this->hasEBO)
        {
            glGenBuffers(1, &EBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(
                GL_ELEMENT_ARRAY_BUFFER,
                this->indexData.size() * sizeof(unsigned int),
                &(this->indexData[0]),
                GL_STATIC_DRAW
            );
        }

        // Assign vertex attributes to VAO.
        int attribID = 0;
        GLsizei offset = 0;
        for (auto const &name : this->fieldNames)
        {
            GLsizei fieldWidth = this->fieldWidths[name];
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
    }

};
}

#endif
