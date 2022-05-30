#ifndef _THUMBNAIL_1635927924764_H
#define _THUMBNAIL_1635927924764_H
#include "thumbnail/exporter.h"
#include <vector>
#include<string>

namespace trimesh
{
	class TriMesh;
}

THUMBNAIL_API void thumbnail_trimeshes(const std::vector<trimesh::TriMesh*>& meshes, int width, int height, unsigned char* data);
THUMBNAIL_API void thumbnail_trimesh(trimesh::TriMesh* mesh, int width, int height, unsigned char* data);

THUMBNAIL_API bool thumbnail_trimeshs(const std::vector<trimesh::TriMesh*>& meshes, int width, int height, int model_color_idx, const char* filePath);
THUMBNAIL_API bool thumbnail_trimesh(trimesh::TriMesh* mesh, int width, int height, int model_color_idx, const char* filePath);
THUMBNAIL_API bool thumbnail_to_gcode(const std::vector<unsigned char>& inPrevData, const std::string& inImgSizes, const std::string& inImgFormat, const std::string& imgPixelSE, const int& inlayerCount, std::vector<std::string>& outGcodeStr);
//
THUMBNAIL_API bool thumbnail_image2base64(const std::vector<unsigned char>& inPrevData, const std::string& inImgSizes, const std::string& inImgFormat, std::vector<std::string>& outGcodeStr);
THUMBNAIL_API bool thumbnail_base2image(const std::vector<std::string>& inPrevData, std::vector<unsigned char>& outGcodeStr);

#endif // _THUMBNAIL_1635927924764_H