#ifndef _NULLSPACE_TOPOMESH_1589114605621_H
#define _NULLSPACE_TOPOMESH_1589114605621_H

#include "triangle.h"
#include "aabb.h"

struct Vertex
{
    Vec3 vertex;
    Vec3 normal;
};

struct Tri
{
    int i[3];
};

class TopoMesh
{
public:
    TopoMesh();
    ~TopoMesh();

    void loadFromMesh(const Mesh& mesh);

    std::vector<Vertex> vertexes;
    std::vector<Tri> tris;
    AABBox box;
};

#endif // _NULLSPACE_TOPOMESH_1589114605621_H
