#include "stl_thumbnail.h"
#include "raster_backend/rasterconfig.h"
#include "raster_backend/raster.h"
#include "stl/parser.h"
#include "picture.h"
#include "cxbin/load.h"
#include "cxbin/convert.h"
#include "trimesh2/TriMesh.h"

//TODO :test memory
//#ifdef _WIN32
//#include <Windows.h>
//#include <Psapi.h>
//#endif
//void showMemory()
//{
//#ifdef _WIN32
//    HANDLE handle = GetCurrentProcess();
//    PROCESS_MEMORY_COUNTERS pmc;
//    GetProcessMemoryInfo(handle, &pmc, sizeof(pmc));
//    //
//    std::cout << "内存使用:" << pmc.WorkingSetSize / 1000 << "K/" << pmc.PeakWorkingSetSize / 1000 << "K + " << pmc.PagefileUsage / 1000 << "K/" << pmc.PeakPagefileUsage / 1000 << "K" << std::endl;
//#endif
//}

int STLThumbnailTrimesh(const std::string& stl_file_path, const std::string& thumbnail_file_path,
                 size_t thumbnail_width, size_t thumbnail_height, int model_color_idx, const Vec4& back_color)
{
    //showMemory();
    static Vec3 model_colors[] =
    {
        {0x42 / 255.f, 0xB5 / 255.f, 1.0f}, // 浅蓝
        {0x45 / 255.f, 0xBA / 255.f, 0xAB / 255.f}, // 浅绿
        {0.0f, 0x99 / 255.f, 0.0f}, // 深绿
        {0xFF / 255.f, 0x99 / 255.f, 0x33 / 255.f}, // 桔黄
        {0xCC / 255.f, 0x66 / 255.f, 0x99 / 255.f} // 粉红
    };

    if (model_color_idx < 0 || model_color_idx >= (int) (sizeof(model_colors) / sizeof(Vec3)))
    {
        model_color_idx = rand() % 5;
    }

    RasterConfig raster_config;
    raster_config.input = stl_file_path;
    raster_config.output = thumbnail_file_path;
    raster_config.picWidth = thumbnail_width;
    raster_config.picHeight = thumbnail_height;
    raster_config.modelColor = model_colors[model_color_idx];
//    rasterConfig.viewDir = Vec3(-1.0f, -1.0f, -1.0f);
//    rasterConfig.viewRight = Vec3(1.0f, -1.0f, 0.0f);

    stl::Parser stl_parser;
    stl_parser.setCalculateVolume(false);
    Mesh mesh;

    std::vector<trimesh::TriMesh*> meshs = cxbin::loadT(raster_config.input, nullptr);
    if (0== meshs.size())
    {
		//std::cout << "mesh is null." << std::endl;
		return -1;
    }
    //showMemory();
    for (trimesh::TriMesh* aMesh : meshs)
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
    //showMemory();

    cxbin::release(meshs);

    //    if (stl_parser.parseFile(mesh, raster_config.input) != 0)
    //    {
    ////        std::cout << "parse stl file " << rasterConfig.input << " failed." << std::endl;
    //        return -1;
    //    }

    if (0 == mesh.size())
    {
        //std::cout << "mesh is null." << std::endl;
        return -1;
    }

    Picture picture(raster_config.picWidth, raster_config.picHeight, 4); // 4:rgba
    picture.setBg(nullptr, back_color);

    Raster raster;
    if (!raster.raster(&picture, &mesh, &raster_config))
    {
//        std::cout << "raster error." << std::endl;
        return -1;
    }

    if (picture.save(raster_config.output) != 0)
    {
//        std::cout << "save file error ." << rasterConfig.output << std::endl;
        return -1;
    }
    //showMemory();
    return 0;
}

int STLThumbnail(const std::string& stl_file_path, const std::string& thumbnail_file_path,
    size_t thumbnail_width, size_t thumbnail_height, int model_color_idx, const Vec4& back_color)
{
    //showMemory();
    static Vec3 model_colors[] =
    {
        {0x42 / 255.f, 0xB5 / 255.f, 1.0f}, // 浅蓝
        {0x45 / 255.f, 0xBA / 255.f, 0xAB / 255.f}, // 浅绿
        {0.0f, 0x99 / 255.f, 0.0f}, // 深绿
        {0xFF / 255.f, 0x99 / 255.f, 0x33 / 255.f}, // 桔黄
        {0xCC / 255.f, 0x66 / 255.f, 0x99 / 255.f} // 粉红
    };

    if (model_color_idx < 0 || model_color_idx >= (int)(sizeof(model_colors) / sizeof(Vec3)))
    {
        model_color_idx = rand() % 5;
    }

    RasterConfig raster_config;
    raster_config.input = stl_file_path;
    raster_config.output = thumbnail_file_path;
    raster_config.picWidth = thumbnail_width;
    raster_config.picHeight = thumbnail_height;
    raster_config.modelColor = model_colors[model_color_idx];
    //    rasterConfig.viewDir = Vec3(-1.0f, -1.0f, -1.0f);
    //    rasterConfig.viewRight = Vec3(1.0f, -1.0f, 0.0f);

    stl::Parser stl_parser;
    stl_parser.setCalculateVolume(false);

    trimesh::TriMesh* mesh = cxbin::loadAll(raster_config.input, nullptr);
    if (!mesh)
    {
        //std::cout << "mesh is null." << std::endl;
        return -1;
    }

    //showMemory();
    Picture picture(raster_config.picWidth, raster_config.picHeight, 4); // 4:rgba
    picture.setBg(nullptr, back_color);

    Raster raster;
    if (!raster.rasterTriMesh(&picture, mesh, &raster_config))
    {
        //        std::cout << "raster error." << std::endl;
        return -1;
    }

    if (picture.save(raster_config.output) != 0)
    {
        //        std::cout << "save file error ." << rasterConfig.output << std::endl;
        return -1;
    }

    if (true)
    {
        delete mesh;
        mesh = nullptr;
    }
    //showMemory();
   return 0;
}

static void MatrixMultiplyPoint(Vec3& point, const float matrix[])
{
    const float x = matrix[0] * point.x + matrix[4] * point.y + matrix[8] * point.z;
    const float y = matrix[1] * point.x + matrix[5] * point.y + matrix[9] * point.z;
    const float z = matrix[2] * point.x + matrix[6] * point.y + matrix[10] * point.z;

    point.x = x;
    point.y = y;
    point.z = z;
}

int CalcTargetModelSize(Vec3& size, const std::string& stl_file_path, const float matrix[])
{
    RasterConfig raster_config;
    raster_config.input = stl_file_path;

    stl::Parser stl_parser;
    stl_parser.setCalculateVolume(false);
    Mesh mesh;

    if (stl_parser.parseFile(mesh, raster_config.input) != 0)
    {
//        std::cout << "parse stl file " << rasterConfig.input << " failed." << std::endl;
        return -1;
    }

    if (0 == mesh.size())
    {
//        std::cout << "mesh is null." << std::endl;
        return -1;
    }

    for (auto& triangle : mesh)
    {
        MatrixMultiplyPoint(triangle.vertices[0], matrix);
        MatrixMultiplyPoint(triangle.vertices[1], matrix);
        MatrixMultiplyPoint(triangle.vertices[2], matrix);
    }

    AABBox aabb = AABBox(mesh);
    size = aabb.size();

    return 0;
}
