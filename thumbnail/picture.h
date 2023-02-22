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
#include "thumbnail/exporter.h"
#include <string>
#include <vector>
#include <png.h> // sudo apt-get install libpng-dev
#include "vec4.h"

using Byte   = unsigned char;
using Buffer = std::vector<Byte>;

class THUMBNAIL_API Picture
{
public:
    explicit Picture(size_t width, size_t height, int depth = 4); // depth=3: rgb, depth=4: rgba

    void setBg(const char* bg_file_path, const Vec4& bg_color = Vec4());

    int save(const std::string& file_path);
    void setRGB(size_t x, size_t y, float r, float g, float b, float a = 1.0f);

    size_t width()
    {
        return width_;
    }
    size_t height()
    {
        return height_;
    }
    int depth()
    {
        return depth_;
    }

    void copyto(unsigned char* data, int depth = 4); // depth=3: rgb, depth=4: rgba
    static int getSELine(const std::string& file_path, int& sline, int& eline);
private:
    void fill(float r, float g, float b, float a);
    void setBg(png_byte color_type, png_bytep* row_pointers, const Vec4& bg_color);
    void setRGB(size_t x, size_t y, Byte r, Byte g, Byte b, Byte a = 255);

    int compress(const std::string& file_path);

private:
    size_t width_ = 0;
    size_t height_ = 0;

    std::string bg_file_path_;
    Vec4 bg_color_ = { 0xFB / 255.f, 0xF7 / 255.f, 0xF4 / 255.f, 0.0f }; // 背景色。alpha值为0表示透明，为1表示不透明，值越小越透明

    int depth_  = 4; // rgba
    size_t stride_ = 0;
    Buffer buffer_;
};
