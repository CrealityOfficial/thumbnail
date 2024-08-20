#include "rasterconfig.h"
//#include <iostream>

RasterConfig::RasterConfig()
    : picWidth(800)
    , picHeight(800)
{
    backColor = Vec4(0xFB / 255.f, 0xF7 / 255.f, 0xF4 / 255.f, 0.0f); //

    lightPos = Vec3(2.0f, -2.0f, 2.5f);

    ambientColor = Vec3(0.4f, 0.4f, 0.4f); // 
    diffuseColor = Vec3(0.2f, 0.2f, 0.2f); // 
    specularColor = Vec3(0.7f, 0.7f, 0.7f); // 

    modelColor = { 0x42 / 255.f, 0xB5 / 255.f, 1.0f };

    viewDir = Vec3(0.0f, 1.0f, -1.0f);
    viewRight = Vec3(1.0f, 0.0f, 0.0f);

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
