#ifndef CLASS_F
#define CLASS_F

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <cmath>


class Coord;
class Vertex;
class Edge;
class Face;
class Object;

class Coord
{
public:
	GLfloat x;
    GLfloat y;
    GLfloat z;

	Coord() : x(0.0f), y(0.0f), z(0.0f) {}
	Coord(GLfloat _x, GLfloat _y, GLfloat _z) : x(_x), y(_y), z(_z) {}

	void add(const Coord& other)
	{
		this->x += other.x;
		this->y += other.y;
		this->z += other.z;
	}

	void sub(const Coord& other)
	{
		this->x -= other.x;
		this->y -= other.y;
		this->z -= other.z;
	}

	void mul(GLfloat factor)
	{
		this->x *= factor;
		this->y *= factor;
		this->z *= factor;
	}

	void div(GLfloat divider)
	{
		this->x /= divider;
		this->y /= divider;
		this->z /= divider;
	}

	void normalize()
	{
		GLfloat size = (GLfloat)sqrt(
            (double)this->x * this->x
            + (double)this->y * this->y
            + (double)this->z * this->z
        );
		this->x /= size;
		this->y /= size;
		this->z /= size;
	}
};


class Vertex
{
public:
	Coord xyz;
    Coord avg_norm;
	std::vector<Edge *> *eList;
	std::vector<Face *> *fList;
	int idx;
	Vertex *vNew;

	~Vertex()
	{
		delete eList;
		delete fList;
		delete vNew;
	}
};


class Edge
{
public:
	std::vector<Face *> *fList;
	Vertex *v1;
    Vertex *v2;
	Vertex *ePoint;
	Coord avg;

	~Edge()
	{
		delete fList;
		delete v1;
		delete v2;
		delete ePoint;
	}
};


class Face
{
public:
	std::vector<Vertex *> *vList;
	std::vector<Edge *> *eList;
	Coord norm;
	Vertex *fPoint;

	~Face()
	{
		delete vList;
		delete eList;
		delete fPoint;
	}
};


class Object
{
public:
	int numVerticesPerFace;

	std::vector<Vertex *> *vList;
	std::vector<Edge *> *eList;
	std::vector<Face *> *fList;
	std::vector<GLfloat> *vertices;
	std::vector<unsigned int> *vertexIndices;

	~Object()
	{
		delete vList;
		delete eList;
		delete fList;
		delete vertices;
		delete vertexIndices;
	}
};

Object *cube();
Object *catmull_clark(Object *obj);

#endif
