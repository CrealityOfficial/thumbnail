#ifndef _THUMBNAIL_1635927924764_H
#define _THUMBNAIL_1635927924764_H
#include "thumbnail/exporter.h"
#include <vector>
#include<string>
#include "trimesh2/TriMesh.h"

class Picture;

void thumbnail_trimeshes(const std::vector<trimesh::TriMesh*>& meshes, int width, int height, unsigned char* data);
void thumbnail_trimesh_convert(trimesh::TriMesh* mesh, int width, int height, unsigned char* data);

void thumbnail_trimesh_convert(trimesh::TriMesh* mesh, int width, int height, unsigned char* data, int stride, trimesh::vec3 viewDir, trimesh::vec3 viewRight);

THUMBNAIL_API void thumbnail_trimesh(trimesh::TriMesh* mesh, int width, int height, unsigned char* data, int stride, trimesh::vec3 viewDir, trimesh::vec3 viewRight);//stride 3 RGB 4 RGBA ; 

THUMBNAIL_API void thumbnail_trimesh(trimesh::TriMesh* mesh, int width, int height, unsigned char* datad);

THUMBNAIL_API bool thumbnail_trimesh_not_convert_op(Picture* picture,const std::vector<trimesh::vec3>& triangles, const trimesh::box3& aabb, int width, int height, int model_color_idx, const char* filePath);

THUMBNAIL_API int thumbnail_to_getSE(const char* infilePath, int& s, int& e);
THUMBNAIL_API bool thumbnail_trimeshs(const std::vector<trimesh::TriMesh*>& meshes, int width, int height, int model_color_idx, const char* filePath);
THUMBNAIL_API bool thumbnail_trimesh_not_convert(trimesh::TriMesh* mesh, int width, int height, int model_color_idx, const char* filePath);
THUMBNAIL_API bool thumbnail_trimesh(trimesh::TriMesh* mesh, int width, int height, int model_color_idx, const char* filePath);
THUMBNAIL_API bool thumbnail_to_gcode(const std::vector<unsigned char>& inPrevData, const std::string& inImgSizes, const std::string& inImgFormat,
	const std::string& imgPixelSE, const int& inlayerCount, std::vector<std::string>& outGcodeStr, const float& layerHeight = 0.);
//
THUMBNAIL_API bool thumbnail_image2base64(const std::vector<unsigned char>& inPrevData, const std::string& inImgSizes, const std::string& inImgFormat, std::vector<std::string>& outGcodeStr);
THUMBNAIL_API bool thumbnail_base2image(const std::vector<std::string>& inPrevData, std::vector<unsigned char>& outGcodeStr);

#endif // _THUMBNAIL_1635927924764_H