#include "topomesh.h"
#include <float.h>
#include <map>

class point_cmp
{
public:
    point_cmp(float e = FLT_MIN) : eps(e) {}

    bool operator()(const Vec3& v0, const Vec3& v1) const
    {
        if (fabs(v0.x - v1.x) <= eps)
        {
            if (fabs(v0.y - v1.y) <= eps)
            {
                return (v0.z < v1.z - eps);
            }
            else
            {
                return (v0.y < v1.y - eps);
            }
        }
        else
        {
            return (v0.x < v1.x - eps);
        }
    }
private:
    float eps;
};

TopoMesh::TopoMesh()
{
}

TopoMesh::~TopoMesh()
{
}

void TopoMesh::loadFromMesh(const Mesh& mesh)
{
    box = AABBox(mesh);

    typedef std::map<Vec3, unsigned, point_cmp> unique_point;
    typedef unique_point::iterator point_iterator;

    unique_point points;

    unsigned faceNum = (unsigned)mesh.size();
    vertexes.reserve(3 * faceNum);
    tris.resize(faceNum);

    for (unsigned i = 0; i < faceNum; ++i)
    {
        const Triangle& triangle = mesh.at(i);
        Tri& tri = tris.at(i);
        Vec3 n = triangle.calcNormal();
        n.normalized();

        for (unsigned j = 0; j < 3; ++j)
        {
            Vec3 p = triangle.vertices.at(j);
            point_iterator it = points.find(p);
            if (it != points.end())
            {
                unsigned index = (*it).second;
                tri.i[j] = index;
                vertexes[index].normal = vertexes[index].normal + n;
            }
            else
            {
                unsigned index = (unsigned)points.size();
                points.insert(unique_point::value_type(p, index));
                tri.i[j] = index;
                vertexes.push_back(Vertex());

                vertexes.back().vertex = p;
            }
        }
    }

    size_t verSize = vertexes.size();
    for (size_t i = 0; i < verSize; ++i)
    {
        vertexes.at(i).normal.normalized();
    }
}

