#include "thumbnail.h"
#include "raster_backend/rasterconfig.h"
#include "raster_backend/raster.h"
#include "picture.h"
#include "trimesh2/TriMesh.h"
#include"img2gcode.h"
static Vec3 model_colors[] =
{
    {0x42 / 255.f, 0xB5 / 255.f, 1.0f}, // ǳ��
    {0x45 / 255.f, 0xBA / 255.f, 0xAB / 255.f}, // ǳ��
    {0.0f, 0x99 / 255.f, 0.0f}, // ����
    {0xFF / 255.f, 0x99 / 255.f, 0x33 / 255.f}, // �ۻ�
    {0xCC / 255.f, 0x66 / 255.f, 0x99 / 255.f} // �ۺ�
};


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

void thumbnail_trimeshes(const std::vector<trimesh::TriMesh*>& meshes, int width, int height, unsigned char* data)
{
    if (width <= 0 || height <= 0 || !data)
        return;
    int model_color_idx = 1;
    if (model_color_idx < 0 || model_color_idx >= (int)(sizeof(model_colors) / sizeof(Vec3)))
    {
        model_color_idx = rand() % 5;
    }

    RasterConfig raster_config;
    raster_config.output = "test.png";
    raster_config.picWidth = width;
    raster_config.picHeight = height;
    raster_config.modelColor = model_colors[model_color_idx];
    //    rasterConfig.viewDir = Vec3(-1.0f, -1.0f, -1.0f);
    //    rasterConfig.viewRight = Vec3(1.0f, -1.0f, 0.0f);

    Mesh mesh;
    _convert(meshes, mesh);

    if (0 == mesh.size())
    {
        //std::cout << "mesh is null." << std::endl;
        return;
    }

    Picture picture(raster_config.picWidth, raster_config.picHeight, 4); // 4:rgba
    picture.setBg(nullptr, Vec4());

    Raster raster;
    if (!raster.raster(&picture, &mesh, &raster_config))
    {
        //        std::cout << "raster error." << std::endl;
        return;
    }

    picture.copyto(data);
    if (picture.save(raster_config.output) != 0)
    {
        //        std::cout << "save file error ." << rasterConfig.output << std::endl;
        return;
    }
}

void thumbnail_trimesh(trimesh::TriMesh* mesh, int width, int height, unsigned char* data)
{
	if (!mesh)
		return;

	std::vector<trimesh::TriMesh*> meshes;
	meshes.push_back(mesh);

	thumbnail_trimeshes(meshes, width, height, data);
}

bool thumbnail_trimeshs(const std::vector<trimesh::TriMesh*>& meshes, int width, int height, int model_color_idx,const char* filePath)
{
    if (width <= 0 || height <= 0 || !filePath)
        return false;
      if (model_color_idx < 0 || model_color_idx >= (int)(sizeof(model_colors) / sizeof(Vec3)))
    {
        model_color_idx = rand() % 5;
    }

    RasterConfig raster_config;
    raster_config.output = filePath;
    raster_config.picWidth = width;
    raster_config.picHeight = height;
    raster_config.modelColor = model_colors[model_color_idx];
    //    rasterConfig.viewDir = Vec3(-1.0f, -1.0f, -1.0f);
    //    rasterConfig.viewRight = Vec3(1.0f, -1.0f, 0.0f);
    Mesh mesh;
    _convert(meshes, mesh);

    if (0 == mesh.size())
    {
        return false;
    }

    Picture picture(raster_config.picWidth, raster_config.picHeight, 4); // 4:rgba
    picture.setBg(nullptr, Vec4());

    Raster raster;
    if (!raster.raster(&picture, &mesh, &raster_config))
    {
        return false;
    }
    if (picture.save(raster_config.output) != 0)
    {
        //        std::cout << "save file error ." << rasterConfig.output << std::endl;
        return false;
    }
    return true;
}
bool  thumbnail_trimesh(trimesh::TriMesh* mesh, int width, int height, int model_color_idx, const char* filePath)
{
    return thumbnail_trimeshs({ mesh }, width, height, model_color_idx,filePath);
}

bool thumbnail_to_gcode(const char* infilePath, const int inImgSizes, const std::string& inImgFormat, const int& inlayerCount, std::string& outGcodeStr)
{
    if (!infilePath) return false;
    
}
bool thumbnail_to_gcode(const std::vector<unsigned char>& inPrevData, const std::string& inImgSizes, const std::string& inImgFormat, const int& inlayerCount, std::vector<std::string>& outGcodeStr)
{
   return  Img2Gcode::imgEncode(inPrevData,outGcodeStr,inImgSizes, inImgFormat, inlayerCount, nullptr);
}