#ifndef CLASS_F
#define CLASS_F

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <cmath>
#include <iostream>
#include <iomanip>
#include <vector>


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
    Coord(const Coord &_coord) : x(_coord.x), y(_coord.y), z(_coord.z) {}

	Coord &add(const Coord &other)
	{
		this->x += other.x;
		this->y += other.y;
		this->z += other.z;
        return *this;
	}

	Coord &sub(const Coord &other)
	{
		this->x -= other.x;
		this->y -= other.y;
		this->z -= other.z;
        return *this;
	}

	Coord &mul(GLfloat factor)
	{
		this->x *= factor;
		this->y *= factor;
		this->z *= factor;
        return *this;
	}

	Coord &div(GLfloat divider)
	{
		this->x /= divider;
		this->y /= divider;
		this->z /= divider;
        return *this;
	}

    Coord operator+(const Coord &other)
    {
        Coord res;
        res.x = this->x + other.x;
        res.y = this->y + other.y;
        res.z = this->z + other.z;
        return res;
    }

    Coord operator-(const Coord &other)
    {
        Coord res;
        res.x = this->x - other.x;
        res.y = this->y - other.y;
        res.z = this->z - other.z;
        return res;
    }

    Coord operator*(GLfloat factor)
    {
        Coord res;
        res.x = this->x * factor;
        res.y = this->y * factor;
        res.z = this->z * factor;
        return res;
    }

    Coord operator/(GLfloat divider)
    {
        Coord res;
        res.x = this->x / divider;
        res.y = this->y / divider;
        res.z = this->z / divider;
        return res;
    }

    Coord &operator+=(const Coord &other)
    {
        this->x += other.x;
        this->y += other.y;
        this->z += other.z;
        return *this;
    }

    Coord &operator-=(const Coord &other)
    {
        return *this;
    }

	Coord &normalize()
	{
		GLfloat size = (GLfloat)sqrt(
            (double)this->x * this->x
            + (double)this->y * this->y
            + (double)this->z * this->z
        );
		this->x /= size;
		this->y /= size;
		this->z /= size;
        return *this;
	}

    void print(bool newline = true, unsigned int precision = 3, unsigned int width = 7)
    {
        std::cout << std::setw(width) << std::setprecision(precision)
            << this->x << "  " << this->y << "  " << this->z;
        if (newline)
        {
            std::cout << std::endl;
        }
    }
};


class Vertex
{
public:
	int idx;

	Coord xyz;
    Coord avgNorm;

	std::vector<Edge *> *eList;
	std::vector<Face *> *fList;
	Vertex *vNew;

	~Vertex()
	{
		delete eList;
		delete fList;
		delete vNew;
	}

    void print(const char *label = nullptr, bool verbose = false, bool newline = true, unsigned int precision = 3, unsigned int width = 7)
    {
        if (label)
        {
            std::cout << std::setw(10) << std::string(label) << "  ";
        }
        std::cout << std::setw(3) << std::setprecision(precision)
            << "Vertex ID : " << this->idx << std::setw(width)
            << "   Coord  : " << this->xyz.x << "  " << this->xyz.y << "  " << this->xyz.z
            << "   avgNorm : " << this->avgNorm.x << "  " << this->avgNorm.y << "  " << this->avgNorm.z;
        if (verbose)
        {
            if (this->fList)
            {
                std::cout << std::setw(3)
                << "   # Faces : " << this->fList->size();
            }
            if (this->eList)
            {
                std::cout << std::setw(3)
                << "   # Edges : " << this->eList->size();
            }
            if (this->vNew)
            {
                this->vNew->print(nullptr, false, false);
            }
        }
        if (newline)
        {
            std::cout << std::endl;
        }
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
	Vertex *fPoint;

	Coord norm;

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
Object *subdivCatmullClark(Object *obj);

#endif
