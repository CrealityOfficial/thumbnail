/*
Copyright (C) 2017  Paul Kremer

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "parser.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <fstream>
#include "helpers.h"

// STL format specifications: http://www.fabbers.com/tech/STL_Format
#define STL_HEADER_SIZE 80
#define STL_FACET_SIZE 50

namespace stl
{
Parser::Parser()
    : calculateVolume(true)
{
    facet_count_ = 0;
    volume_ = 0.0;
}

Parser::~Parser()
{
}

void Parser::setCalculateVolume(bool enable)
{
    calculateVolume = enable;
}

bool Parser::IsBinarySTL(std::ifstream& in, size_t fileSize)
{
    if (fileSize < 84)
    {
        return false;
    }

    in.seekg(STL_HEADER_SIZE);
    uint32_t faceCount = readU32(in);
    const uint32_t expectedBinaryFileSize = faceCount * 50 + 84;
    return expectedBinaryFileSize == fileSize;
}

int Parser::parseFile(Mesh& mesh, const std::string& file_path)
{
    std::ifstream stream(file_path, std::ifstream::in | std::ifstream::binary);
    if (!stream)
    {
//        throw ("Cannot open file");
        return -1;
    }

    struct stat stat_buf;
    stat(file_path.c_str(), &stat_buf);

    if (IsBinarySTL(stream, stat_buf.st_size))
    {
        return parseBinary(mesh, stream, stat_buf.st_size);
    }

    return parseAscii(mesh, stream, stat_buf.st_size);
}

//bool Parser::isBinaryFormat(std::ifstream& in) const
//{
//    // Note: A file starting with "solid" is no indicator for having an ASCII file
//    // Some exporters put "solid <name>" in the binary header

//    std::string line;
//    getTrimmedLine(in, line); // skip potential string: solid <name>
//    getTrimmedLine(in, line); // has to start with "facet" otherwise it is a binary file

//    // return to begin of file
//    in.clear();
//    in.seekg(0, in.beg);

//    return line.substr(0, 5) != "facet";
//}

int Parser::parseBinary(Mesh& mesh, std::ifstream& in, size_t file_size)
{
    // skip header
    in.seekg(STL_HEADER_SIZE); // 文件起始的80个字节是文件头，用于存贮零件名

    // get the number of triangles in the stl 紧接着用4个字节的整数来描述模型的三角面片个数
    const uint32_t triangle_count = readU32(in);
    mesh.reserve(triangle_count); // 太大了内存可能会爆掉

    // parse triangles 后面逐个给出每个三角面片的几何信息
    for (size_t i = 0; i < triangle_count && in; ++i)
    {
        Triangle triangle;
        readBinaryTriangle(triangle, in);
        mesh.emplace_back(triangle);
    }

    return 0;
}

bool Parser::IsAsciiSTL(std::ifstream& in, unsigned int fileSize)
{
    // Note: A file starting with "solid" is no indicator for having an ASCII file
    // Some exporters put "solid <name>" in the binary header
    in.seekg(0);

    std::string line;
    getTrimmedLine(in, line); // skip potential string: solid <name>

    if (line.substr(0, 5) != "solid")
    {
        return false;
    }

    getTrimmedLine(in, line); // has to start with "facet" otherwise it is a binary file
    return line.substr(0, 5) == "facet";

//    in.seekg(0, in.beg);
//    int nNumWhiteSpace = 0;
//    unsigned char nWhiteSpace = 0;
//    in.read(reinterpret_cast<char*>(&nWhiteSpace), 1);
//    while (isspace(nWhiteSpace))
//    {
//        ++nNumWhiteSpace;
//        in.read(reinterpret_cast<char*>(&nWhiteSpace), 1);
//    }
//    in.seekg(nNumWhiteSpace, std::ios::beg);
//    char chBuffer[6];
//    in.read(reinterpret_cast<char*>(chBuffer), 5);
//    chBuffer[5] = '\0';
//    bool isASCII = false;
//    if (!strcmp(chBuffer, "solid") || !strcmp(chBuffer, "Solid") || !strcmp(chBuffer, "SOLID"))
//    {
//        isASCII = true;
//    }
//    in.seekg(0, in.beg);
//    unsigned char b[1];
//    if (isASCII)
//    {
//        if (fileSize >= 500)
//        {
//            isASCII = true;
//            for (unsigned int i = 0; i < 500; i++)
//            {
//                in.read(reinterpret_cast<char*>(b), 1);
//                if (b[0] > 127)
//                {
//                    isASCII = false;
//                    break;
//                }
//            }
//        }
//    }
//    return isASCII;
}

int Parser::parseAscii(Mesh& mesh, std::ifstream& in, size_t fileSize)
{
    if (!IsAsciiSTL(in, fileSize))
    {
        return false;
    }

    in.seekg(0);

    // solid name
    std::string line;
    getTrimmedLine(in, line);

    while (in)
    {
        Triangle triangle;
        bool got_triangle = true;

        if (readAsciiTriangle(triangle, got_triangle, in) != 0)
        {
            return -1;
        }

        if (got_triangle)
        {
            mesh.emplace_back(triangle);
        }
    }

    return 0;
}

uint32_t Parser::readU32(std::ifstream& in) const
{
    uint32_t v;
    in.read(reinterpret_cast<char*>(&v), sizeof(v));
    return v;
}

uint16_t Parser::readU16(std::ifstream& in) const
{
    uint16_t v;
    in.read(reinterpret_cast<char*>(&v), sizeof(v));
    return v;
}

float Parser::readFloat(std::ifstream& in) const
{
    float v;
    in.read(reinterpret_cast<char*>(&v), sizeof(v));
    return v;
}

int Parser::readVector3(Vec3& v, std::ifstream& in) const
{
    v.x = readFloat(in);
    v.y = readFloat(in);
    v.z = readFloat(in);

    return 0;
}

int Parser::readAsciiTriangle(Triangle& triangle, bool& got_triangle, std::ifstream& in)
{
    std::string line = "";

    getTrimmedLine(in, line);
    int n = std::sscanf(line.c_str(), "facet normal %e %e %e", &triangle.normal.x, &triangle.normal.y, &triangle.normal.z);
    if (n != 3)
    {
        got_triangle = false;

        if (0 == line.length() || "endsolid" == line.substr(0, 8) || "end solid" == line.substr(0, 9) || "end" == line.substr(0, 3))
        {
            // 要么是endsolid，要么是文件结束
            return 0;
        }
        else if ("solid" == line.substr(0, 5))
        {
            // 新的一个。这种情况是一个stl文件中有多个solid xx和endsolid
            return 0;
        }

        return -1;
    }

//    triangle.normal.normalize();

    getTrimmedLine(in, line); // outer loop

    for (size_t i = 0; i < 3; ++i)
    {
        getTrimmedLine(in, line);
        n = std::sscanf(line.c_str(), "vertex %e %e %e", &triangle.vertices[i].x, &triangle.vertices[i].y, &triangle.vertices[i].z);
        if (n != 3)
        {
            return -1;
        }
    }

    getTrimmedLine(in, line); // endloop
    getTrimmedLine(in, line); // endfacet

#ifdef SWAP_STL_01
    Vec3 p = triangle.vertices[0];
    triangle.vertices[0] = triangle.vertices[1];
    triangle.vertices[1] = p;
#endif

//    if ((0 == triangle.normal.x && 0 == triangle.normal.y && 0 == triangle.normal.z)
//            || std::isnan(triangle.normal.x) || std::isnan(triangle.normal.y) || std::isnan(triangle.normal.z))
//    {
    triangle.normal = triangle.calcNormal().normalize(); // 有些模型的法向量不对，这里自己算一遍
//    }

    ++facet_count_;

    if (calculateVolume)
    {
        volume_ += signedVolumeOfTriangle(triangle.vertices[0], triangle.vertices[1], triangle.vertices[2]);
    }

    return 0;
}

// https://stackoverflow.com/questions/1406029/how-to-calculate-the-volume-of-a-3d-mesh-object-the-surface-of-which-is-made-up
// https://n-e-r-v-o-u-s.com/blog/?p=4415
double Parser::signedVolumeOfTriangle(const Vec3& p1, const Vec3& p2, const Vec3& p3)
{
    double v321 = p3.x * p2.y * p1.z;
    double v231 = p2.x * p3.y * p1.z;
    double v312 = p3.x * p1.y * p2.z;
    double v132 = p1.x * p3.y * p2.z;
    double v213 = p2.x * p1.y * p3.z;
    double v123 = p1.x * p2.y * p3.z;
    return (1.0 / 6.0) * (-v321 + v231 + v312 - v132 - v213 + v123);
}

void Parser::readBinaryTriangle(Triangle& triangle, std::ifstream& in)
{
    // 每个三角面片占用固定的50个字节，依次是3个4字节浮点数(角面片的法矢量)，
    // 3个4字节浮点数(1个顶点的坐标)，3个4字节浮点数(2个顶点的坐标)，3个4字节浮点数(3个顶点的坐标)，
    // 最后2个字节用来描述三角面片的属性信息
    readVector3(triangle.normal, in);
//    triangle.normal.normalize();

#ifdef SWAP_STL_01
    readVector3(triangle.vertices[1], in);
    readVector3(triangle.vertices[0], in);
#else
    readVector3(triangle.vertices[0], in);
    readVector3(triangle.vertices[1], in);
#endif
    readVector3(triangle.vertices[2], in);
    readU16(in); // attributes

    // some stl files have garbage normals
    // we recalculate them here in case they are NaN
//    if ((0 == triangle.normal.x && 0 == triangle.normal.y && 0 == triangle.normal.z)
//            || std::isnan(triangle.normal.x) || std::isnan(triangle.normal.y) || std::isnan(triangle.normal.z))
//    {
    triangle.normal = triangle.calcNormal().normalize(); // 有些模型的法向量不对，这里自己算一遍
//    }

    ++facet_count_;

    if (calculateVolume)
    {
        volume_ += signedVolumeOfTriangle(triangle.vertices[0], triangle.vertices[1], triangle.vertices[2]);
    }
}
} // namespace
