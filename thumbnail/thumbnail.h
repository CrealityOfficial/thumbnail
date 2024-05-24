#ifndef _THUMBNAIL_1635927924764_H
#define _THUMBNAIL_1635927924764_H
#include "thumbnail/exporter.h"
#include <vector>
#include<string>
#include "trimesh2/TriMesh.h"

class Picture;
class ZBuffer;

THUMBNAIL_API bool thumbnail_trimesh_2_picture_index(Picture* picture, ZBuffer* buffer, trimesh::TriMesh* mesh, const trimesh::box3& aabb, int width, int height, int model_color_idx);
THUMBNAIL_API bool thumbnail_trimesh_2_picture(Picture* picture, ZBuffer* buffer, trimesh::TriMesh* mesh, const trimesh::box3& aabb, int width, int height, const trimesh::vec3& color);
THUMBNAIL_API bool thumbnail_triangles_2_picture_index(Picture* picture, ZBuffer* buffer, const std::vector<trimesh::vec3>& triangles, const trimesh::box3& aabb, int width, int height, int color_index);
THUMBNAIL_API bool thumbnail_triangles_2_picture(Picture* picture, ZBuffer* buffer, const std::vector<trimesh::vec3>& triangles, const trimesh::box3& aabb, int width, int height, const trimesh::vec3& color);

THUMBNAIL_API bool thumbnail_trimeshes(const std::vector<trimesh::TriMesh*>& meshes, int width, int height, const trimesh::vec3& color, const std::string& out);
THUMBNAIL_API bool thumbnail_trimeshes_index(const std::vector<trimesh::TriMesh*>& meshes, int width, int height, int color_index, const std::string& out);
THUMBNAIL_API bool thumbnail_trimesh(trimesh::TriMesh* mesh, int width, int height, const trimesh::vec3& color, const std::string& out);
THUMBNAIL_API bool thumbnail_trimesh_index(trimesh::TriMesh* mesh, int width, int height, int color_index, const std::string& out);
//ÔÆÇÐÆ¬µ÷ÓÃ
THUMBNAIL_API bool thumbnail_trimeshs_gcode_head(const std::vector<trimesh::TriMesh*>& meshes, const float layer_height, const int layer_num, const std::string preImgFormat,
    int width, int height, int model_color_idx, std::string& out);

#endif // _THUMBNAIL_1635927924764_H