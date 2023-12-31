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

#include <stddef.h>
#include <vector>

class ZBuffer
{
public:
    explicit ZBuffer(size_t width, size_t height);
    bool testAndSet(size_t x, size_t y, float z);

    void setTestMethod(bool useLess);

private:
    size_t width_ = 0;
    size_t height_ = 0;
    std::vector<float> buffer_;

    bool less_;
};
