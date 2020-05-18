#include "class.h"

#include <iostream>


Object *initObject()
{
	Object *newObj = new Object();
	newObj->vList = new std::vector<Vertex *>();
	newObj->eList = new std::vector<Edge *>();
	newObj->fList = new std::vector<Face *>();
	newObj->vertices = new std::vector<GLfloat>();
	newObj->vertexIndices = new std::vector<unsigned int>();
	return newObj;
}


Vertex *initVertex()
{
	Vertex *newVtx = new Vertex();
	newVtx->eList = new std::vector<Edge *>();
	newVtx->fList = new std::vector<Face *>();
	newVtx->vNew = nullptr;
	newVtx->idx = -1;
	return newVtx;
}


Edge *initEdge()
{
	Edge *newEdge = new Edge();
	newEdge->fList = new std::vector<Face *>();
	newEdge->v1 = nullptr;
	newEdge->v2 = nullptr;
	newEdge->ePoint = nullptr;
	return newEdge;
}


Face *initFace()
{
	Face *newFace = new Face();
	newFace->vList = new std::vector<Vertex *>();
	newFace->eList = new std::vector<Edge *>();
	newFace->fPoint = nullptr;
	return newFace;
}


Vertex *addVertex(Object *obj, const Coord &coord)
{
	Vertex *newVtx = initVertex();
	newVtx->xyz.x = coord.x;
	newVtx->xyz.y = coord.y;
	newVtx->xyz.z = coord.z;
	newVtx->idx = obj->vList->size();
	obj->vList->push_back(newVtx);
	return newVtx;
}


Vertex *addVertex(Object *obj, Vertex *vtx)
{
	vtx->idx = obj->vList->size();
	obj->vList->push_back(vtx);
	return vtx;
}


Edge *findEdge(Object *obj, Vertex *v1, Vertex *v2)
{
    for (auto const &edge : *(v1->eList))
	{
		if (edge->v1 == v2 || edge->v2 == v2)
		{
			return edge;
		}
	}
	return nullptr;
}


Edge *addEdge(Object *obj, Vertex *v1, Vertex *v2)
{
	Edge *newEdge = initEdge();
	newEdge->v1 = v1;
	newEdge->v2 = v2;
	v1->eList->push_back(newEdge);
	v2->eList->push_back(newEdge);
	obj->eList->push_back(newEdge);
	return newEdge;
}


Face *addFace(Object *obj, const std::vector<int> &vertexIndices)
{
	Face *newFace = initFace();
	int n = vertexIndices.size();
	for (int i = 0; i < n; i++)
	{
		Vertex *v1 = (*(obj->vList))[vertexIndices[i]];
		Vertex *v2 = (*(obj->vList))[vertexIndices[(i + 1) % n]];
		v1->fList->push_back(newFace);

		Edge *temp = findEdge(obj, v1, v2);
		if (!temp) temp = addEdge(obj, v1, v2);

		temp->fList->push_back(newFace);
		newFace->eList->push_back(temp);
		newFace->vList->push_back(v1);
	}
	obj->fList->push_back(newFace);
	return newFace;
}


Coord add(const Coord &coord1, const Coord &coord2)
{
	Coord temp;
	temp.x = coord1.x + coord2.x;
	temp.y = coord1.y + coord2.y;
	temp.z = coord1.z + coord2.z;
	return temp;
}


Coord sub(const Coord &coord1, const Coord &coord2)
{
	Coord temp;
	temp.x = coord1.x - coord2.x;
	temp.y = coord1.y - coord2.y;
	temp.z = coord1.z - coord2.z;
	return temp;
}


Coord mul(const Coord &coord1, GLfloat factor)
{
	Coord temp;
	temp.x = coord1.x * factor;
	temp.y = coord1.y * factor;
	temp.z = coord1.z * factor;
	return temp;
}


Coord div(const Coord &coord1, GLfloat divider)
{
	Coord temp;
	temp.x = coord1.x / divider;
	temp.y = coord1.y / divider;
	temp.z = coord1.z / divider;
	return temp;
}


Coord cross(const Coord &coord1, const Coord &coord2)
{
	Coord temp;
	temp.x = coord1.y * coord2.z - coord1.z * coord2.y;
	temp.y = coord1.z * coord2.x - coord1.x * coord2.z;
	temp.z = coord1.x * coord2.y - coord1.y * coord2.x;
	return temp;
}


void setNorm(Object *obj)
{
    for (auto const &face : *(obj->fList))
	{
		Coord v01 = sub((*(face->vList))[1]->xyz, (*(face->vList))[0]->xyz);
		Coord v12 = sub((*(face->vList))[2]->xyz, (*(face->vList))[1]->xyz);
		Coord crs = cross(v01, v12);
		crs.normalize();
		face->norm = crs;
	}

    for (auto const &vtx : *(obj->vList))
    {
		Coord sum;
        for (auto const &face : *(vtx->fList))
        {
            sum.add(face->norm);
        }
		sum.div((GLfloat)(vtx->fList->size()));
		sum.normalize();
		vtx->avgNorm = sum;
	}
}


void aggregateVertices(Object *obj)
{
	obj->vertices->clear();
	obj->vertexIndices->clear();

    for (auto const &vtx : *(obj->vList))
	{
		Coord temp_pos = vtx->xyz;
		Coord temp_norm = vtx->avgNorm;
		obj->vertices->push_back(temp_pos.x);
		obj->vertices->push_back(temp_pos.y);
		obj->vertices->push_back(temp_pos.z);
		obj->vertices->push_back(temp_norm.x);
		obj->vertices->push_back(temp_norm.y);
		obj->vertices->push_back(temp_norm.z);
	}

    // Triangle face.
	if (obj->numVerticesPerFace == 3)
	{
        for (auto const &face : *(obj->fList))
		{
			obj->vertexIndices->push_back((*face->vList)[0]->idx);
			obj->vertexIndices->push_back((*face->vList)[1]->idx);
			obj->vertexIndices->push_back((*face->vList)[2]->idx);
		}
	}

    // Quad face.
	else if (obj->numVerticesPerFace == 4)
	{
        for (auto const &face : *(obj->fList))
		{
			obj->vertexIndices->push_back((*face->vList)[0]->idx);
			obj->vertexIndices->push_back((*face->vList)[1]->idx);
			obj->vertexIndices->push_back((*face->vList)[2]->idx);
			obj->vertexIndices->push_back((*face->vList)[2]->idx);
			obj->vertexIndices->push_back((*face->vList)[3]->idx);
			obj->vertexIndices->push_back((*face->vList)[0]->idx);
		}
	}
}


Object *cube()
{
	Object *newObj = initObject();
	newObj->numVerticesPerFace = 4;
	for (int x = -1; x <= 1; x += 2)
	{
		for (int y = -1; y <= 1; y += 2)
		{
			for (int z = -1; z <= 1; z += 2)
			{
				addVertex(newObj, Coord((GLfloat)x, (GLfloat)y, (GLfloat)z));
			}
		}
	}

	addFace(newObj, { 0, 2, 6, 4 });
	addFace(newObj, { 0, 4, 5, 1 });
	addFace(newObj, { 0, 1, 3, 2 });
	addFace(newObj, { 2, 3, 7, 6 });
	addFace(newObj, { 6, 7, 5, 4 });
	addFace(newObj, { 1, 5, 7, 3 });
    
	setNorm(newObj);
	aggregateVertices(newObj);
	return newObj;
}


Object *donut()
{
    Object *newObj = initObject();
    newObj->numVerticesPerFace = 4;

    addVertex(newObj, Coord(-1.5f,   0.5f, -1.5f));  // 0
    addVertex(newObj, Coord(-1.5f,   0.5f,  1.5f));  // 1
    addVertex(newObj, Coord( 1.5f,   0.5f, -1.5f));  // 2
    addVertex(newObj, Coord( 1.5f,   0.5f,  1.5f));  // 3
    addVertex(newObj, Coord(-1.5f,  -0.5f, -1.5f));  // 4
    addVertex(newObj, Coord(-1.5f,  -0.5f,  1.5f));  // 5
    addVertex(newObj, Coord( 1.5f,  -0.5f, -1.5f));  // 6
    addVertex(newObj, Coord( 1.5f,  -0.5f,  1.5f));  // 7

    addVertex(newObj, Coord(-0.75f,  0.5f, -0.75f)); // 8
    addVertex(newObj, Coord(-0.75f,  0.5f,  0.75f)); // 9
    addVertex(newObj, Coord( 0.75f,  0.5f, -0.75f)); // 10
    addVertex(newObj, Coord( 0.75f,  0.5f,  0.75f)); // 11
    addVertex(newObj, Coord(-0.75f, -0.5f, -0.75f)); // 12
    addVertex(newObj, Coord(-0.75f, -0.5f,  0.75f)); // 13
    addVertex(newObj, Coord( 0.75f, -0.5f, -0.75f)); // 14
    addVertex(newObj, Coord( 0.75f, -0.5f,  0.75f)); // 15

    addFace(newObj, {  0,  1,  9,  8 });
    addFace(newObj, {  1,  2, 10,  9 });
    addFace(newObj, {  2,  3, 11, 10 });
    addFace(newObj, {  3,  0,  8, 11 });

    addFace(newObj, {  4,  5, 13, 12 });
    addFace(newObj, {  5,  6, 14, 13 });
    addFace(newObj, {  6,  7, 15, 14 });
    addFace(newObj, {  7,  8, 12, 15 });

    addFace(newObj, {  0,  1,  5,  4 });
    addFace(newObj, {  1,  2,  6,  5 });
    addFace(newObj, {  2,  3,  7,  6 });
    addFace(newObj, {  3,  0,  4,  7 });

    addFace(newObj, {  8,  9, 13, 12 });
    addFace(newObj, {  9, 10, 14, 13 });
    addFace(newObj, { 10, 11, 15, 14 });
    addFace(newObj, { 11,  8, 12, 15 });

	setNorm(newObj);
	aggregateVertices(newObj);
	return newObj;
}


bool isHoleEdge(Edge *e)
{
    return e->fList->size() == 1;
}


bool isHoleVertex(Vertex *v)
{
    return v->fList->size() != v->eList->size();
}


Vertex *facePoint(Face *f)
{
    // Face point already exists.
    if (f->fPoint)
        return f->fPoint;

    // New face point is generated as the average of all vertices belongs to
    // the face.
    Vertex *newVtx = initVertex();
    int nVertices = f->vList->size();
    for (auto const &vtx : *(f->vList))
    {
        newVtx->xyz += vtx->xyz;
    }
    newVtx->xyz.div(nVertices);
    f->fPoint = newVtx;
    return newVtx;
}


Vertex *edgePoint(Edge *e)
{
    // Edge point already exists.
    if (e->ePoint)
        return e->ePoint;

    // New edge point is generated as the average of all face points of the
    // faces which the edge belongs to, and the two endpoints of the edge.
    Vertex *newVtx = initVertex();
    int nFaces = e->fList->size();
    for (auto const &face : *(e->fList))
    {
        newVtx->xyz += facePoint(face)->xyz;
    }
    newVtx->xyz += (e->v1->xyz + e->v2->xyz);
    newVtx->xyz.div(nFaces + 2);
    e->ePoint = newVtx;
    return newVtx;
}


Vertex *vertexPoint(Vertex *v)
{
    // Precalculated vNew exists.
    if (v->vNew)
        return v->vNew;

    Vertex *newVtx = initVertex();

    if (isHoleVertex(v))
    {
        // Average of the midpoints of holeEdges which v belongs to and the v.
        int counter = 1;
        for (auto const &edge : *(v->eList))
        {
            if (isHoleEdge(edge))
            {
                ++counter;
                newVtx->xyz += (edge->v1->xyz + edge->v2->xyz).mul(0.5f);
            }
        }
        newVtx->xyz += v->xyz;
        newVtx->xyz.div(counter);
    }
    else
    {
        // Coordinates are evaluated as follows:
        // n : # faces which v belongs to.
        // coord := avg_face_points/n + 2*avg_mid_points/n + (n-3)*v_coord/n
        int n = v->fList->size();

        Coord xyz1;
        for (auto const &face: *(v->fList))
        {
            Vertex *fPoint = facePoint(face);
            xyz1 += fPoint->xyz;
        }
        xyz1.div(n);

        Coord xyz2;
        for (auto const &edge : *(v->eList))
        {
            xyz2 += (edge->v1->xyz + edge->v2->xyz).mul(0.5f);
        }
        xyz2.mul(2.0f / n);

        Coord xyz3 = v->xyz * (GLfloat)(n - 3);
        newVtx->xyz = (xyz1 + xyz2 + xyz3).div((GLfloat)n);
    }
    v->vNew = newVtx;
    return newVtx;
}


Object *subdivCatmullClark(Object *obj)
{
	Object *newObj = initObject();
	newObj->numVerticesPerFace = obj->numVerticesPerFace;

    for (auto const &face : *(obj->fList))
    {
        std::vector<Vertex *> &vList = *(face->vList);
        int nVertices = vList.size();

        // Triangle faces: 
        if (nVertices == 3)
        {
            {
                Vertex *a = vList[0];
                Vertex *b = vList[1];
                Vertex *c = vList[2];

                Vertex *vA = vertexPoint(a);
                Vertex *vB = vertexPoint(b);
                Vertex *vC = vertexPoint(c);
                Vertex *eAB = edgePoint(findEdge(obj, a, b));
                Vertex *eBC = edgePoint(findEdge(obj, b, c));
                Vertex *eCA = edgePoint(findEdge(obj, c, a));
                Vertex *f = facePoint(face);

                Vertex *vAn = addVertex(newObj, vA);
                Vertex *vBn = addVertex(newObj, vB);
                Vertex *vCn = addVertex(newObj, vC);
                Vertex *eABn = addVertex(newObj, eAB);
                Vertex *eBCn = addVertex(newObj, eBC);
                Vertex *eCAn = addVertex(newObj, eCA);
                Vertex *fn = addVertex(newObj, f);

                addFace(newObj, { vAn->idx, eABn->idx, fn->idx, eCAn->idx });
                addFace(newObj, { vBn->idx, eBCn->idx, fn->idx, eABn->idx });
                addFace(newObj, { vCn->idx, eCAn->idx, fn->idx, eBCn->idx });

                // Debug log
                std::cout << "Original face points" << std::endl;
                a->print("A");
                b->print("B");
                c->print("C");

                std::cout << "New face points" << std::endl;
                vA->print("vA");
                vB->print("vB");
                vC->print("vC");
                eAB->print("eAB");
                eBC->print("eBC");
                eCA->print("eCA");
                f->print("fABC");

                std::cout << std::endl;
            }
        }

        // Quadrilateral faces:
        else if (nVertices == 4)
        {
            {
                Vertex *a = vList[0];
                Vertex *b = vList[1];
                Vertex *c = vList[2];
                Vertex *d = vList[3];

                Vertex *vA = vertexPoint(a);
                Vertex *vB = vertexPoint(b);
                Vertex *vC = vertexPoint(c);
                Vertex *vD = vertexPoint(d);
                Vertex *eAB = edgePoint(findEdge(obj, a, b));
                Vertex *eBC = edgePoint(findEdge(obj, b, c));
                Vertex *eCD = edgePoint(findEdge(obj, c, d));
                Vertex *eDA = edgePoint(findEdge(obj, d, a));
                Vertex *f = facePoint(face);

                Vertex *vAn = addVertex(newObj, vA);
                Vertex *vBn = addVertex(newObj, vB);
                Vertex *vCn = addVertex(newObj, vC);
                Vertex *vDn = addVertex(newObj, vD);
                Vertex *eABn = addVertex(newObj, eAB);
                Vertex *eBCn = addVertex(newObj, eBC);
                Vertex *eCDn = addVertex(newObj, eCD);
                Vertex *eDAn = addVertex(newObj, eDA);
                Vertex *fn = addVertex(newObj, f);

                addFace(newObj, { vAn->idx, eABn->idx, fn->idx, eDAn->idx });
                addFace(newObj, { vBn->idx, eBCn->idx, fn->idx, eABn->idx });
                addFace(newObj, { vCn->idx, eCDn->idx, fn->idx, eBCn->idx });
                addFace(newObj, { vDn->idx, eDAn->idx, fn->idx, eCDn->idx });

                // Debug log
                std::cout << "Original face points" << std::endl;
                a->print("A");
                b->print("B");
                c->print("C");
                d->print("D");

                std::cout << "New face points" << std::endl;
                vA->print("vA");
                vB->print("vB");
                vC->print("vC");
                vD->print("vD");
                eAB->print("eAB");
                eBC->print("eBC");
                eCD->print("eCD");
                eDA->print("eDA");
                f->print("fABCD");

                std::cout << std::endl;
            }
        }
    }

	setNorm(newObj);
	aggregateVertices(newObj);

	delete obj;
	return newObj;
}
