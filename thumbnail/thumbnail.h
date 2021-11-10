#ifndef _THUMBNAIL_1635927924764_H
#define _THUMBNAIL_1635927924764_H
#include "thumbnail/exporter.h"
#include <vector>

namespace trimesh
{
	class TriMesh;
}

THUMBNAIL_API void thumbnail_trimeshes(const std::vector<trimesh::TriMesh*>& meshes, int width, int height, unsigned char* data);
THUMBNAIL_API void thumbnail_trimesh(trimesh::TriMesh* mesh, int width, int height, unsigned char* data);

THUMBNAIL_API bool thumbnail_trimeshs(const std::vector<trimesh::TriMesh*>& meshes, int width, int height, int model_color_idx, const char* filePath);
THUMBNAIL_API bool thumbnail_trimesh(trimesh::TriMesh* mesh, int width, int height, int model_color_idx, const char* filePath);
#endif // _THUMBNAIL_1635927924764_H