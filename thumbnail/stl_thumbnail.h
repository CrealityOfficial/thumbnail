#pragma once
#include "thumbnail/exporter.h"
#include <string>
#include "thumbnail/vec3.h"
#include "thumbnail/vec4.h"

//int STLThumbnail(const char* stl_file_path, const char* thumbnail_file_path,
//                 size_t thumbnail_width, size_t thumbnail_height, const char* bg_file_path, int depth);
//int STLThumbnails(std::vector<std::string>& thumbnail_file_path_vec, const char* stl_file_path, const char* thumbnail_file_path_prefix,
//                  size_t thumbnail_width, size_t thumbnail_height, const char* bg_file_path, int depth);

//int STLThumbnailAll(const char* stl_file_path, const char* thumbnail_file_path,
//                    size_t thumbnail_width, size_t thumbnail_height, const char* thumbnail_bg_file_path,
//                    std::vector<std::string>& covers_file_path_vec, const char* covers_file_path_prefix,
//                    size_t covers_width, size_t covers_height, const char* covers_bg_file_path, int depth,
//                    double& volume, uint32_t& facet_count, double& x, double& y, double& z);

// model_color_idx为-1时，颜色随机
THUMBNAIL_API int STLThumbnail(const std::string& stl_file_path, const std::string& thumbnail_file_path,
                 size_t thumbnail_width, size_t thumbnail_height, int model_color_idx, const Vec4& back_color);

// 计算原始模型经过变换之后的size
int CalcTargetModelSize(Vec3& size, const std::string& stl_file_path, const float matrix[16]);

// TODO convert ascii stl to binary
