#ifndef _NULLSPACE_RASTER_1588967160928_H
#define _NULLSPACE_RASTER_1588967160928_H

#include "thumbnail/topomesh.h"

namespace trimesh
{
    class TriMesh;
}

class Picture;
class RasterConfig;
class Raster
{
public:
    Raster();
    ~Raster();

    bool raster(Picture* picture, Mesh* mesh, RasterConfig* rasterConfig);
    bool rasterTriMesh(Picture* picture, trimesh::TriMesh* mesh, RasterConfig* rasterConfig);
    bool raster(Picture* picture, TopoMesh* topoMesh, RasterConfig* rasterConfig);

protected:
};

#endif // _NULLSPACE_RASTER_1588967160928_H
