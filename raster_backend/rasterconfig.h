#ifndef _NULLSPACE_RASTERCONFIG_1589000611373_H
#define _NULLSPACE_RASTERCONFIG_1589000611373_H

#include "vec3.h"
#include "vec4.h"
#include <string>

class RasterConfig
{
public:
    RasterConfig();
    ~RasterConfig();

//    void loadFromArg(int argc, char** argv);

    int picWidth;
    int picHeight;

    Vec4 backColor;

    Vec3 viewDir;
    Vec3 viewRight;
    Vec3 viewUp;

    Vec3 lightPos;

    Vec3 diffuseColor;
    Vec3 specularColor;
    Vec3 ambientColor;

    Vec3 modelColor;

    std::string input;
    std::string output;
};

#endif // _NULLSPACE_RASTERCONFIG_1589000611373_H
