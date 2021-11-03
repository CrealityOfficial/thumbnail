#include "rasterconfig.h"
//#include <iostream>

RasterConfig::RasterConfig()
    : picWidth(800)
    , picHeight(800)
{
    backColor = Vec4(0xFB / 255.f, 0xF7 / 255.f, 0xF4 / 255.f, 0.0f); // 背景色。alpha值为0表示透明，为1表示不透明，值越小越透明;

    lightPos = Vec3(2.0f, -2.0f, 2.5f);

    ambientColor = Vec3(0.4f, 0.4f, 0.4f); // 环境光
    diffuseColor = Vec3(0.2f, 0.2f, 0.2f); // 漫反射光
    specularColor = Vec3(0.7f, 0.7f, 0.7f); // 镜面反射光

    modelColor = { 0x42 / 255.f, 0xB5 / 255.f, 1.0f };

//    如果 viewDir (0.0 , -1.0, -1.0) viewRight(1.0, 0.0, 0.0)
//    是后面， 前面应该是 viewDir(0.0, 1.0, -1.0) viewRight(-1.0, 0.0, 0.0)
    viewDir = Vec3(0.0f, 2.0f, -1.0f);
    viewRight = Vec3(-1.0f, 0.0f, 0.0f);

    viewDir = viewDir.normalize();
    viewRight = viewRight.normalize();

    viewUp = cross(viewRight, viewDir);
}

RasterConfig::~RasterConfig()
{

}

//void RasterConfig::loadFromArg(int argc, char** argv)
//{
//    if (argc < 3)
//    {
//        std::cout << "1. input .stl file\n"
//                  << "2. output .png file\n"
//                  << std::endl;
//    }

//    if (argc >= 3)
//    {
//        input = argv[1];
//        output = argv[2];
//    }
//}
