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

#include "aabb.h"
#include <limits>

AABBox::AABBox()
{
}

AABBox::AABBox(const Mesh& mesh)
{
    lower = { std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max() };
    upper = { std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest() };

    for (const auto& triangle : mesh)
    {
        lower.x = std::min(std::min(lower.x, triangle.vertices[0].x), std::min(triangle.vertices[1].x, triangle.vertices[2].x));
        lower.y = std::min(std::min(lower.y, triangle.vertices[0].y), std::min(triangle.vertices[1].y, triangle.vertices[2].y));
        lower.z = std::min(std::min(lower.z, triangle.vertices[0].z), std::min(triangle.vertices[1].z, triangle.vertices[2].z));

        upper.x = std::max(std::max(upper.x, triangle.vertices[0].x), std::max(triangle.vertices[1].x, triangle.vertices[2].x));
        upper.y = std::max(std::max(upper.y, triangle.vertices[0].y), std::max(triangle.vertices[1].y, triangle.vertices[2].y));
        upper.z = std::max(std::max(upper.z, triangle.vertices[0].z), std::max(triangle.vertices[1].z, triangle.vertices[2].z));
    }
}
