#include "thumbnail.h"
#include "raster_backend/rasterconfig.h"
#include "raster_backend/raster.h"
#include "picture.h"
#include "trimesh2/TriMesh.h"
#include "img2gcode.h"

#include "raster_backend/zbuffer.h"

static Vec3 model_colors[] =
{
    {0x42 / 255.f, 0xB5 / 255.f, 1.0f}, // 浅蓝
    {0x45 / 255.f, 0xBA / 255.f, 0xAB / 255.f}, // 浅绿
    {0.0f, 0x99 / 255.f, 0.0f}, // 深绿
    {0xFF / 255.f, 0x99 / 255.f, 0x33 / 255.f}, // 桔黄
    {0xCC / 255.f, 0x66 / 255.f, 0x99 / 255.f}, // 粉红
    {0x80 / 255.f, 0x80 / 255.f, 0x80 / 255.f} // 灰色
};

trimesh::vec3 index_2_color(int color_index) {
    if (color_index < 0 || color_index >= (int)(sizeof(model_colors) / sizeof(Vec3)))
    {
        color_index = rand() % 5;
    }

    Vec3 c = model_colors[color_index];
    return trimesh::vec3(c.x, c.y, c.z);
}

trimesh::TriMesh* mergeMeshes(const std::vector<trimesh::TriMesh*>& inMeshes)
{
    trimesh::TriMesh* outMesh = new trimesh::TriMesh();
    size_t totalVertexSize = outMesh->vertices.size();
    size_t totalUVSize = outMesh->cornerareas.size();
    size_t totalTriangleSize = outMesh->faces.size();

    size_t addVertexSize = 0;
    size_t addTriangleSize = 0;
    size_t addUVSize = 0;

    size_t meshSize = inMeshes.size();
    for (size_t i = 0; i < meshSize; ++i)
    {
        if (inMeshes.at(i))
        {
            addVertexSize += inMeshes.at(i)->vertices.size();
            addTriangleSize += inMeshes.at(i)->faces.size();
            addUVSize += inMeshes.at(i)->cornerareas.size();
        }
    }
    totalVertexSize += addVertexSize;
    totalTriangleSize += addTriangleSize;
    totalUVSize += addUVSize;

    if (addVertexSize > 0 && addTriangleSize > 0)
    {
        outMesh->vertices.reserve(totalVertexSize);
        outMesh->cornerareas.reserve(totalUVSize);
        outMesh->faces.reserve(totalTriangleSize);

        size_t startFaceIndex = outMesh->faces.size();
        size_t startVertexIndex = outMesh->vertices.size();;
        size_t startUVIndex = outMesh->cornerareas.size();
        for (size_t i = 0; i < meshSize; ++i)
        {
            trimesh::TriMesh* mesh = inMeshes.at(i);
            if (mesh)
            {
                int vertexNum = (int)mesh->vertices.size();
                int faceNum = (int)mesh->faces.size();
                int uvNum = (int)mesh->cornerareas.size();
                if (vertexNum > 0 && faceNum > 0)
                {
                    outMesh->vertices.insert(outMesh->vertices.end(), mesh->vertices.begin(), mesh->vertices.end());
                    outMesh->cornerareas.insert(outMesh->cornerareas.end(), mesh->cornerareas.begin(), mesh->cornerareas.end());
                    outMesh->faces.insert(outMesh->faces.end(), mesh->faces.begin(), mesh->faces.end());

                    size_t endFaceIndex = startFaceIndex + faceNum;
                    if (startVertexIndex > 0)
                    {
                        for (size_t ii = startFaceIndex; ii < endFaceIndex; ++ii)
                        {
                            trimesh::TriMesh::Face& face = outMesh->faces.at(ii);
                            for (int j = 0; j < 3; ++j)
                                face[j] += startVertexIndex;
                        }
                    }

                    startFaceIndex += faceNum;
                    startVertexIndex += vertexNum;
                    startUVIndex += uvNum;

                }
            }
        }
    }

    return outMesh;
}

void _convert(const std::vector<trimesh::TriMesh*>& meshes, Mesh& mesh)
{
    for (trimesh::TriMesh* aMesh : meshes)
    {
        for (size_t i = 0; i < aMesh->faces.size(); i++)
        {
            int index0 = aMesh->faces[i].at(0);
            int index1 = aMesh->faces[i].at(1);
            int index2 = aMesh->faces[i].at(2);
            Triangle atriangle;
            atriangle.vertices.at(0).x = aMesh->vertices[index0].at(0);
            atriangle.vertices.at(0).y = aMesh->vertices[index0].at(1);
            atriangle.vertices.at(0).z = aMesh->vertices[index0].at(2);

            atriangle.vertices.at(1).x = aMesh->vertices[index1].at(0);
            atriangle.vertices.at(1).y = aMesh->vertices[index1].at(1);
            atriangle.vertices.at(1).z = aMesh->vertices[index1].at(2);

            atriangle.vertices.at(2).x = aMesh->vertices[index2].at(0);
            atriangle.vertices.at(2).y = aMesh->vertices[index2].at(1);
            atriangle.vertices.at(2).z = aMesh->vertices[index2].at(2);
            atriangle.normal = atriangle.calcNormal();
            atriangle.normal.normalized();
            mesh.push_back(atriangle);
        }
    }
}

std::string floatToString(const float& dbNum)
{
    char* chCode;
    chCode = new(std::nothrow)char[20];
    sprintf(chCode, "%.2f", dbNum);
    std::string strCode(chCode);
    delete[]chCode;
    return strCode;
}

bool thumbnail_trimesh_2_picture_index(Picture* picture, ZBuffer* buffer, trimesh::TriMesh* mesh, const trimesh::box3& aabb, int width, int height, int color_index)
{
    return thumbnail_trimesh_2_picture(picture, buffer, mesh, aabb, width, height, index_2_color(color_index));
}

bool thumbnail_trimesh_2_picture(Picture* picture, ZBuffer* buffer, trimesh::TriMesh* mesh, const trimesh::box3& aabb, int width, int height, const trimesh::vec3& color)
{
    std::vector<trimesh::vec3> triangles;
    if (mesh)
    {
        for (const trimesh::TriMesh::Face& face : mesh->faces)
        {
            triangles.push_back(mesh->vertices.at(face.x));
            triangles.push_back(mesh->vertices.at(face.y));
            triangles.push_back(mesh->vertices.at(face.z));
        }
    }

    return thumbnail_triangles_2_picture(picture, buffer, triangles, aabb, width, height, color);
}

bool thumbnail_triangles_2_picture_index(Picture* picture, ZBuffer* buffer, const std::vector<trimesh::vec3>& triangles, const trimesh::box3& aabb, int width, int height, int color_index)
{
    return thumbnail_triangles_2_picture(picture, buffer, triangles, aabb, width, height, index_2_color(color_index));
}

bool thumbnail_triangles_2_picture(Picture* picture, ZBuffer* buffer, const std::vector<trimesh::vec3>& triangles, const trimesh::box3& aabb, int width, int height, const trimesh::vec3& color)
{
    if (!picture || !buffer || width <= 0 || height <= 0)
    {
        return false;
    }

    if (triangles.empty())
    {
        return false;
    }

    Vec3 c(color.x, color.y, color.z);
    RasterConfig raster_config;
    raster_config.picWidth = width;
    raster_config.picHeight = height;
    raster_config.modelColor = c;

    Raster raster;
    if (raster.rasterTriangles(picture, buffer, triangles, aabb, &raster_config))
        return true;

    return false;
}

int thumbnail_to_getSE(const char* infilePath, int& s, int& e)
{
    return Picture::getSELine(infilePath, s, e);
}

bool thumbnail_to_gcode(const char* infilePath, const int inImgSizes, const std::string& inImgFormat, const int& inlayerCount, std::string& outGcodeStr)
{
    if (!infilePath) return false;
    return true;    
}

bool thumbnail_to_gcode(const std::vector<unsigned char>& inPrevData, const std::string& inImgSizes, const std::string& inImgFormat, 
    const std::string& imgPixelSE, const int& inlayerCount, std::vector<std::string>& outGcodeStr, const float& layerHeight)
{
    std::string imageMeg;
    if (layerHeight != 0.)
    {
        imageMeg = imgPixelSE + " " + std::to_string(inlayerCount);
    }
    return  Img2Gcode::imgEncode(inPrevData, outGcodeStr, inImgFormat, inImgSizes, imageMeg, nullptr);
}

bool thumbnail_image2base64(const std::vector<unsigned char>& inPrevData, const std::string& inImgSizes, const std::string& inImgFormat, std::vector<std::string>& outGcodeStr)
{
	return  Img2Gcode::image2base(inPrevData, inImgSizes, inImgFormat, std::string(), outGcodeStr);
}

bool thumbnail_base2image(const std::vector<std::string>& inPrevData, std::vector<unsigned char>& outGcodeStr)
{
    return  Img2Gcode::base2image(inPrevData, outGcodeStr);
}

void getImageStr(std::string& imageStr, std::string imgSavePath, std::string imgSize, std::string sPreImgFormat, int layers, float layerHeight)
{
    std::string imgSaveStdPath(imgSavePath);
    std::fstream ios(imgSaveStdPath, std::ios::binary | std::ios::in);
    std::string s;
    std::vector<unsigned char> data;
    while (std::getline(ios, s))
    {
        s += "\n";
        int src_size = data.size();
        data.resize(src_size + s.size());
        copy(s.begin(), s.end(), data.begin() + src_size);
    }
    ios.clear();
    ios.close();
    data.pop_back();
    std::vector<std::string> outStr;
    std::string imgPixelSE;
    int sLine = -1, eLine = -1;
    thumbnail_to_getSE(imgSavePath.c_str(), sLine, eLine);
    imgPixelSE = std::to_string(sLine) + " " + std::to_string(eLine);
    thumbnail_to_gcode(data, imgSize, sPreImgFormat, imgPixelSE, layers, outStr, layerHeight);
    for (auto& line : outStr)
    {
        imageStr += line;
        imageStr += "\n";
    }
}

bool thumbnail_trimeshes(const std::vector<trimesh::TriMesh*>& meshes, int width, int height, const trimesh::vec3& color, const std::string& out)
{
    if (meshes.size() == 0)
        return false;

    Picture picture(width, height, 4);
    ZBuffer zbuffer(width, height);

    picture.setBg(nullptr, Vec4());
    zbuffer.setTestMethod(true);

    trimesh::box3 box;
    for (trimesh::TriMesh* mesh : meshes)
    {
        mesh->clear_bbox();
        mesh->need_bbox();
        box += mesh->bbox;
    }
    for (trimesh::TriMesh* mesh : meshes)
    {
        thumbnail_trimesh_2_picture(&picture, &zbuffer, mesh, box, width, height, color);
    }
    picture.save(out);
    return true;
}

bool thumbnail_trimeshes_index(const std::vector<trimesh::TriMesh*>& meshes, int width, int height, int color_index, const std::string& out)
{
    return thumbnail_trimeshes(meshes, width, height, index_2_color(color_index), out);
}

bool thumbnail_trimesh(trimesh::TriMesh* mesh, int width, int height, const trimesh::vec3& color, const std::string& out)
{
    if (!mesh)
        return false;

    std::vector<trimesh::TriMesh*> meshes;
    meshes.push_back(mesh);
    return thumbnail_trimeshes(meshes, width, height, color, out);
}

bool thumbnail_trimesh_index(trimesh::TriMesh* mesh, int width, int height, int color_index, const std::string& out)
{
    return thumbnail_trimesh(mesh, width, height, index_2_color(color_index), out);
}

bool thumbnail_trimeshs_gcode_head(const std::vector<trimesh::TriMesh*>& meshes, const float layer_height, const int layer_num, const std::string preImgFormat, int width, int height, int model_color_idx
    , std::string& out)
{
    if (meshes.size() == 0)
    {
        return false;
    }
    std::string previewImagePath = "test." + preImgFormat;
    std::string imgSize = std::to_string(width) + "*" + std::to_string(height);
    std::string previewImageDataString;
    thumbnail_trimeshes_index(meshes, width, height, model_color_idx, previewImagePath.c_str());
    getImageStr(previewImageDataString, previewImagePath, imgSize, preImgFormat, layer_num, layer_height);
    if (!previewImageDataString.empty())
    {
        out = previewImageDataString;
    }
    return true;
}

   

