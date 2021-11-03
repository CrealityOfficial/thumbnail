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

#pragma once

#include <math.h>
#include "triangle.h"
#include "vec3.h"

namespace stl
{
class Parser
{
public:
    Parser();
    ~Parser();

    int parseFile(Mesh& mesh, const std::string& file_path);

    void setCalculateVolume(bool enable);

    uint32_t facetCount() const
    {
        return facet_count_ ;
    }

    double volume() const
    {
        return fabs(volume_); // 单位：mm^3
    }

private:
//    bool isBinaryFormat(std::ifstream& in) const;
    bool IsBinarySTL(std::ifstream& in, size_t fileSize);
    bool IsAsciiSTL(std::ifstream& in, unsigned int fileSize);

    int parseBinary(Mesh& mesh, std::ifstream& in, size_t file_size);
    int parseAscii(Mesh& mesh, std::ifstream& in, size_t file_size);

    uint32_t readU32(std::ifstream& in) const;
    uint16_t readU16(std::ifstream& in) const;
    float readFloat(std::ifstream& in) const;

    int readVector3(Vec3& vec, std::ifstream& in) const;
    void readBinaryTriangle(Triangle& triangle, std::ifstream& in);
    int readAsciiTriangle(Triangle& triangle, bool& got_triangle, std::ifstream& in);

    double signedVolumeOfTriangle(const Vec3& p1, const Vec3& p2, const Vec3& p3);

private:
    uint32_t facet_count_;
    double volume_;
    bool calculateVolume;
};
} // namespace
