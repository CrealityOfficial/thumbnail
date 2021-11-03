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

#include "zbuffer.h"
#include <limits>

ZBuffer::ZBuffer(size_t width, size_t height)
    : width_(width)
    , height_(height)
    , less_(false)
{
    buffer_.resize(width_ * height_, std::numeric_limits<float>::lowest());
}

bool ZBuffer::testAndSet(size_t x, size_t y, float z)
{
    const size_t i = y * width_ + x;

    if (less_)
    {
        if (z < buffer_[i])
        {
            buffer_[i] = z;
            return true;
        }
    }
    else
    {
        if (z > buffer_[i])
        {
            buffer_[i] = z;
            return true;
        }
    }


    return false;
}

void ZBuffer::setTestMethod(bool useLess)
{
    less_ = useLess;
    if (less_)
    {
        for (size_t i = 0; i < width_; ++i)
        {
            for (size_t j = 0; j < height_; ++j)
            {
                buffer_[j * width_ + i] = std::numeric_limits<float>::max();
            }
        }
    }
}
