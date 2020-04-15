#ifndef RESOURCE_UTILS_H
#define RESOURCE_UTILS_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "json.h"
using json = nlohmann::json;

#include "string_utils.h"


void readFile(std::string path, std::string &data)
{
    data.clear();
    std::ifstream file;
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try 
    {
        file.open(path);
        std::stringstream dataStream;
        dataStream << file.rdbuf();	
        file.close();

        data = dataStream.str();
    }
    catch (std::ifstream::failure e)
    {
        std::cout << "ERROR::RESOURCE_UTILS::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }
}


void readJson(std::string path, json &data)
{
    data.clear();
    std::ifstream file;
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        file.open(path);
        data = json::parse(file);
        file.close();
    }
    catch (json::exception& e)
    {
        std::cout << "ERROR::RESOURCE_UTILS::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
    }
}


void loadSplineControlPoints(const std::string &path, GLsizei &numVertices, std::vector<float> &vertexData)
{
    // Spline file structure
    //
    // N
    // x1 y1 z1
    // x2 y2 z2
    // ...
    // xN yN zN

	// Load spline control point data and return VAO.
    std::string data;
    readFile(path, data);
    if (data.empty())
    {
        std::cout << "ERROR::RESOURCE_UTILS::FILE_IS_EMPTY  "
            << path << std::endl;
    }

    // Initialize the data.
    numVertices = 0;
    vertexData.clear();

    // Parse the data.
    std::string line;
    std::stringstream ss(data);

    // First line is the number of vertices.
    while (std::getline(ss, line, '\n'))
    {
        trim(line);
        if (!line.empty())
            break;

        // We generously allow empty lines at the beginning.
        line.clear();
    }
    numVertices = jr::stoi(line);
    line.clear();

    // Following lines are the vertex coordinates.
    GLsizei idx = 0;
    while (std::getline(ss, line, '\n'))
    {
        trim(line);
        // We generously allow empty lines at the middle.
        if (line.empty())
        {
            line.clear();
            continue;
        }

        // Try to parse the data.
        std::string::size_type sz;
        float x = jr::stof(line, &sz);
        line = line.substr(sz);
        float y = jr::stof(line, &sz);
        line = line.substr(sz);
        float z = jr::stof(line, &sz);
        vertexData.push_back(x);
        vertexData.push_back(y);
        vertexData.push_back(z);

        if (++idx >= numVertices)
            break;
    }
}


void loadBezierSurfaceControlPoints(const std::string &path)
{
	// Load surface control point data and return VAO.
    std::string data;
    readFile(path, data);
    if (data.empty())
    {
        std::cout << "ERROR::RESOURCE_UTILS::FILE_IS_EMPTY  "
            << path << std::endl;
    }

    // Parse the data.
}

#endif
