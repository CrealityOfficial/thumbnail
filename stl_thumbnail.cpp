#include "stl_thumbnail.h"
#include "raster_backend/rasterconfig.h"
#include "raster_backend/raster.h"
#include "stl/parser.h"
#include "picture.h"
#include "cxbin/load.h"
#include "../cxalarm/tracer.h"
#include "trimesh2/TriMesh.h"

//int STLThumbnail(const char* stl_file_path, const char* thumbnail_file_path,
//                 size_t thumbnail_width, size_t thumbnail_height, const char* thumbnail_bg_file_path, int depth)
//{
//    // parse STL
//    stl::Parser stl_parser;
//    Mesh mesh;

//    if (stl_parser.parseFile(mesh, stl_file_path) != 0)
//    {
//        return -1;
//    }

////    std::cout << "Triangles: " << mesh.size() << std::endl;

//    // render using raster backend
//    Picture pic(thumbnail_width, thumbnail_height, depth);
//    pic.setBg(thumbnail_bg_file_path);

//    Vec3 view_pos = { -1.f, -1.f, 1.f };
//    Vec3 light_pos = { 2.f, 2.f, 2.5f };

//    RasterBackend backend(thumbnail_width, thumbnail_height);
//    backend.render(nullptr, pic, mesh, view_pos, light_pos);

//    // save to disk
//    if (pic.save(thumbnail_file_path) != 0)
//    {
//        return -1;
//    }

//    return 0;
//}

//int STLThumbnails(std::vector<std::string>& covers_file_path_vec, const char* stl_file_path, const char* covers_file_path_prefix,
//                  size_t covers_width, size_t covers_height, const char* covers_bg_file_path, int depth)
//{
//    // parse STL
//    stl::Parser stl_parser;
//    Mesh mesh;

//    if (stl_parser.parseFile(mesh, stl_file_path) != 0)
//    {
//        return -1;
//    }

//    const int PIC_COUNT = 4;
//    const Vec3 view_pos_vec[PIC_COUNT] = {{ -1.f, -1.f, 1.f }, { 1.f, -1.f, 1.f }, { -1.f, 1.f, 1.f }, { 1.f, 1.f, 1.f }};
//    const Vec3 light_pos_vec[PIC_COUNT] = {{ 2.f, 2.f, 2.5f }, { -2.f, 2.f, 2.5f }, { 2.f, -2.f, 2.5f }, { -2.f, -2.f, 2.5f }};

//    for (int i = 0; i < PIC_COUNT; ++i)
//    {
//        // render using raster backend
//        Picture pic(covers_width, covers_height, depth);
//        pic.setBg(covers_bg_file_path);

//        RasterBackend backend(covers_width, covers_height);
//        backend.render(nullptr, pic, mesh, view_pos_vec[i], light_pos_vec[i]);

//        // save to disk
//        std::string thumbnail_file_path(covers_file_path_prefix);
//        thumbnail_file_path += "-";
//        thumbnail_file_path.append(std::to_string(i + 1));
//        thumbnail_file_path += ".png";

//        if (pic.save(thumbnail_file_path) != 0)
//        {
//            return -1;
//        }

//        covers_file_path_vec.emplace_back(thumbnail_file_path);
//    }

//    return 0;
//}

//int STLThumbnailAll(const char* stl_file_path, const char* thumbnail_file_path, size_t thumbnail_width, size_t thumbnail_height,
//                    const char* thumbnail_bg_file_path, std::vector<std::string>& covers_file_path_vec, const char* covers_file_path_prefix,
//                    size_t covers_width, size_t covers_height, const char* covers_bg_file_path, int depth,
//                    double& volume, uint32_t& facet_count, double& x, double& y, double& z)
//{
//    // parse STL
//    stl::Parser stl_parser;
//    Mesh mesh;

//    if (stl_parser.parseFile(mesh, stl_file_path) != 0)
//    {
//        return -1;
//    }

//    volume = stl_parser.volume();
//    facet_count = stl_parser.facetCount();

////    std::cout << "Triangles: " << mesh.size() << std::endl;

//    {
//        // render using raster backend
//        Picture pic(thumbnail_width, thumbnail_height, depth);
//        pic.setBg(thumbnail_bg_file_path);

//        Vec3 view_pos = { -1.f, -1.f, 1.f };
//        Vec3 light_pos = { 2.f, 2.f, 2.5f };

//        RasterBackend backend(thumbnail_width, thumbnail_height);
//        Vec3 size;
//        backend.render(&size, pic, mesh, view_pos, light_pos);

//        // save to disk
//        if (pic.save(thumbnail_file_path) != 0)
//        {
//            return -1;
//        }

//        x = size.x;
//        y = size.y;
//        z = size.z;
//    }

//    {
//        const int PIC_COUNT = 4;
//        const Vec3 view_pos_vec[PIC_COUNT] = {{ -1.f, -1.f, 1.f }, { 1.f, -1.f, 1.f }, { -1.f, 1.f, 1.f }, { 1.f, 1.f, 1.f }};
//        const Vec3 light_pos_vec[PIC_COUNT] = {{ 2.f, 2.f, 2.5f }, { -2.f, 2.f, 2.5f }, { 2.f, -2.f, 2.5f }, { -2.f, -2.f, 2.5f }};

//        for (int i = 0; i < PIC_COUNT; ++i)
//        {
//            // render using raster backend
//            Picture pic(covers_width, covers_height, depth);
//            pic.setBg(covers_bg_file_path);

//            RasterBackend backend(covers_width, covers_height);
//            backend.render(nullptr, pic, mesh, view_pos_vec[i], light_pos_vec[i]);

//            // save to disk
//            std::string thumbnail_file_path(covers_file_path_prefix);
//            thumbnail_file_path += "-";
//            thumbnail_file_path.append(std::to_string(i + 1));
//            thumbnail_file_path += ".png";

//            if (pic.save(thumbnail_file_path) != 0)
//            {
//                return -1;
//            }

//            covers_file_path_vec.emplace_back(thumbnail_file_path);
//        }
//    }

//    return 0;
//}

//// 只生成一张缩略图的新接口
//int STLThumbnail2(const std::string& stl_file_path, const std::string& thumbnail_file_path,
//                  size_t thumbnail_width, size_t thumbnail_height, int model_color_idx)
//{
//    // parse STL
//    stl::Parser stl_parser;
//    Mesh mesh;

//    if (stl_parser.parseFile(mesh, stl_file_path) != 0)
//    {
//        return -1;
//    }

////    std::cout << "Triangles: " << mesh.size() << std::endl;

//    static Vec3 model_colors[] =
//    {
//        {0x42 / 255.f, 0xB5 / 255.f, 1.0f}, // 浅蓝
//        {0x45 / 255.f, 0xBA / 255.f, 0xAB / 255.f}, // 浅绿
//        {0.0f, 0x99 / 255.f, 0.0f}, // 深绿
//        {0xFF / 255.f, 0x99 / 255.f, 0x33 / 255.f}, // 桔黄
//        {0xCC / 255.f, 0x66 / 255.f, 0x99 / 255.f} // 粉红
//    };

//    if (model_color_idx < 0 || model_color_idx >= (int) (sizeof(model_colors) / sizeof(Vec3)))
//    {
//        // 随机一个
//        model_color_idx = rand() % (sizeof(model_colors) / sizeof(Vec3));
//    }

//    static Vec3 view_pos = { -1.0f, 2.0f, 1.0f };
//    static Vec3 light_pos = { 2.0f, -2.0f, 2.5f };

//    // render using raster backend
//    Picture pic(thumbnail_width, thumbnail_height);
//    pic.setBg(nullptr);

//    RasterBackend backend(thumbnail_width, thumbnail_height, model_colors[model_color_idx]);
//    backend.render(nullptr, pic, mesh, view_pos, light_pos);

//    // save to disk
//    if (pic.save(thumbnail_file_path) != 0)
//    {
//        return -1;
//    }

//    return 0;
//}

int STLThumbnail(const std::string& stl_file_path, const std::string& thumbnail_file_path,
                 size_t thumbnail_width, size_t thumbnail_height, int model_color_idx, const Vec4& back_color)
{
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

    Mesh mesh;
    CaseTracer tracer("cxbin::LoadT");
    std::vector<trimesh::TriMesh*> meshs = cxbin::loadT(raster_config.input, &tracer);
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
