#include "raster.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "thumbnail/picture.h"
#include "rasterconfig.h"
#include "zbuffer.h"
#include "thumbnail/aabb.h"
#include "thumbnail/helper.h"

Raster::Raster()
{
}
Raster::~Raster()
{
}

bool Raster::rasterTriMesh(Picture* picture, trimesh::TriMesh* mesh, RasterConfig* rasterConfig)
{
    if (!picture || !mesh || !rasterConfig)
    {
        return false;
    }

    size_t width = picture->width();
    size_t height = picture->height();

    if (width == 0 || height == 0)
    {
        return false;
    }

    ZBuffer zbuffer(width, height);
    zbuffer.setTestMethod(true);

    // generate AABB and find its center
    //AABBox aabb = AABBox(*mesh);
    mesh->need_bbox();
    trimesh::box3 aabb = mesh->bbox;

    // create model view projection matrix
    float fovy = 30.0f * 3.1415926f / 180.0f;
    //float len = aabb.size().length() / 2.0f;
    float len = trimesh::len(aabb.max - aabb.min) / 2.0f;


    float d = len / sinf(fovy / 2.0f);
    float nearPlane = d - len;
    float farPlane = d + len;

    glm::highp_mat4 projectionMatrix = glm::perspective(fovy, (float)width / (float)height, nearPlane, farPlane);

    glm::vec3 viewDir = vec3ToGLM(rasterConfig->viewDir);
    glm::vec3 viewUp = vec3ToGLM(rasterConfig->viewUp);
    //glm::vec3 center = vec3ToGLM(aabb.center();
    //mesh->need_bsphere();
    trimesh::vec3 tcenter = aabb.min + (aabb.max - aabb.min) * 0.5f;
    glm::vec3 center = glm::vec3(tcenter.x, tcenter.y, tcenter.z);
    
    glm::vec3 position = center - viewDir * d;

    glm::highp_mat4 viewMatrix = glm::lookAt(position, center, viewUp);
    glm::highp_mat4 modelMatrix;
    glm::highp_mat4 modelViewProjectionMatrix = projectionMatrix * viewMatrix * modelMatrix;

    Vec3 viewPos = Vec3(position.x, position.y, position.z);
    //for (const Triangle& triangle : *mesh)
    int i = -1;
    for (const trimesh::TriMesh::Face& triangle : mesh->faces)
    {
        ++i;
        // project vertices to screen coordinates
        //glm::vec3 v0 = glmMat4x4MulVec3(modelViewProjectionMatrix, vec3ToGLM(triangle.vertices[0]));
        //glm::vec3 v1 = glmMat4x4MulVec3(modelViewProjectionMatrix, vec3ToGLM(triangle.vertices[1]));
        //glm::vec3 v2 = glmMat4x4MulVec3(modelViewProjectionMatrix, vec3ToGLM(triangle.vertices[2]));
        const trimesh::point& p0 = mesh->vertices[triangle[0]];
        const trimesh::point& p1 = mesh->vertices[triangle[1]];
        const trimesh::point& p2 = mesh->vertices[triangle[2]];
        glm::vec3 v0 = glmMat4x4MulVec3(modelViewProjectionMatrix, glm::vec3(p0.x, p0.y, p0.z));
        glm::vec3 v1 = glmMat4x4MulVec3(modelViewProjectionMatrix, glm::vec3(p1.x, p1.y, p1.z));
        glm::vec3 v2 = glmMat4x4MulVec3(modelViewProjectionMatrix, glm::vec3(p2.x, p2.y, p2.z));

        // triangle bounding box
        float min_x = std::min(v0.x, std::min(v1.x, v2.x));
        float min_y = std::min(v0.y, std::min(v1.y, v2.y));
        float max_x = std::max(v0.x, std::max(v1.x, v2.x));
        float max_y = std::max(v0.y, std::max(v1.y, v2.y));

        // bounding box in screen space
        unsigned smin_x = static_cast<unsigned>(std::max(0, static_cast<int>((min_x + 1.0f) / 2.0f * width)));
        unsigned smin_y = static_cast<unsigned>(std::max(0, static_cast<int>((min_y + 1.0f) / 2.0f * height)));
        unsigned smax_x = static_cast<unsigned>(std::max(0, std::min(int(width - 1), static_cast<int>((max_x + 1.0f) / 2.0f * width))));
        unsigned smax_y = static_cast<unsigned>(std::max(0, std::min(int(height - 1), static_cast<int>((max_y + 1.0f) / 2.0f * height))));

        for (unsigned y = smin_y; y <= smax_y; ++y)
        {
            for (unsigned x = smin_x; x <= smax_x; ++x)
            {
                // normalize screen coords [-1,1]
                const float nx = 2.0f * (x / static_cast<float>(width) - 0.5f);
                const float ny = 2.0f * (y / static_cast<float>(height) - 0.5f);

                glm::vec2 P = glm::vec2{ nx, ny };
                glm::vec2 V0 = glm::vec2(v0);
                glm::vec2 V1 = glm::vec2(v1);
                glm::vec2 V2 = glm::vec2(v2);

#if SWAP_STL_01
                bool inside = true;
                float f = edgeFunction(P, V0, V1);
                inside &= (f <= FLT_EPSILON); // ¼´f <= 0.0f
                f = edgeFunction(P, V1, V2);
                inside &= (f <= FLT_EPSILON);
                f = edgeFunction(P, V2, V0);
                inside &= (f <= FLT_EPSILON);

                if (!inside)
                {
                    continue;
                }

                // calculate baricentric coords
                float area = edgeFunction(V0, V1, V2);
                float w0 = edgeFunction(V1, V2, P) / area;
                float w1 = edgeFunction(V2, V0, P) / area;
                float w2 = edgeFunction(V0, V1, P) / area;
#else
                bool inside = true;
                float f = edgeFunction(P, V0, V1);
                inside &= (f >= -FLT_EPSILON); // ¼´f >= 0.0f
                f = edgeFunction(P, V1, V2);
                inside &= (f >= -FLT_EPSILON);
                f = edgeFunction(P, V2, V0);
                inside &= (f >= -FLT_EPSILON);

                if (!inside)
                {
                    continue;
                }

                // calculate baricentric coords
                float area = abs(edgeFunction(V0, V1, V2));
                float w0 = abs(edgeFunction(V1, V2, P)) / area;
                float w1 = abs(edgeFunction(V2, V0, P)) / area;
                float w2 = abs(edgeFunction(V0, V1, P)) / area;
#endif
                // the z position at point p by interpolating the z position of all 3 vertices
                float pz = w0 * v0.z + w1 * v1.z + w2 * v2.z;
                float px = w0 * v0.x + w1 * v1.x + w2 * v2.x;
                float py = w0 * v0.y + w1 * v1.y + w2 * v2.y;

                if (!zbuffer.testAndSet(x, y, pz))
                {
                    continue;
                }

                // calculate lightning
                // diffuse
                Vec3 s2l = rasterConfig->lightPos - Vec3{ px, py, pz };
                //Vec3 diffuse_color = std::max(0.0f, dot(triangle.normal, s2l)) * rasterConfig->diffuseColor;
                trimesh::vec3 sn = trimesh::normalized(mesh->trinorm(i));
                Vec3 vsn = Vec3(sn.x, sn.y, sn.z);
                Vec3 diffuse_color = std::max(0.0f, dot(vsn, s2l)) * rasterConfig->diffuseColor;

                // specular
                Vec3 frag_pos = { px, py, pz };
                Vec3 light_dir = (rasterConfig->lightPos - frag_pos).normalize();
                Vec3 view_dir = (viewPos - frag_pos).normalize();
                //Vec3 reflect_dir = reflect(-light_dir, triangle.normal);
                Vec3 reflect_dir = reflect(-light_dir, vsn);
                Vec3 specular_color = std::pow(std::max(dot(view_dir, reflect_dir), 0.0f), 1.0f) * rasterConfig->specularColor;

                // merge
                Vec3 color(1.0f, 0.0f, 0.0f);
                color = (rasterConfig->ambientColor + diffuse_color + specular_color) * rasterConfig->modelColor;

                // output pixel color
                picture->setRGB(x, y, color.x, color.y, color.z);
            }
        }
    }

    return true;
}

bool Raster::rasterTriangle(Picture* picture, const std::vector<trimesh::vec3>& triangles, const trimesh::box3& aabb, RasterConfig* rasterConfig)
{
    if (!picture || triangles.empty() || !rasterConfig)
    {
        return false;
    }

    size_t width = picture->width();
    size_t height = picture->height();

    if (width == 0 || height == 0)
    {
        return false;
    }

    ZBuffer zbuffer(width, height);
    zbuffer.setTestMethod(true);

    // generate AABB and find its center
    //AABBox aabb = AABBox(*mesh);
    //mesh->need_bbox();
    //trimesh::box3 aabb = mesh->bbox;

    // create model view projection matrix
    float fovy = 30.0f * 3.1415926f / 180.0f;
    //float len = aabb.size().length() / 2.0f;
    float len = trimesh::len(aabb.max - aabb.min) / 2.0f;


    float d = len / sinf(fovy / 2.0f);
    float nearPlane = d - len;
    float farPlane = d + len;

    glm::highp_mat4 projectionMatrix = glm::perspective(fovy, (float)width / (float)height, nearPlane, farPlane);

    glm::vec3 viewDir = vec3ToGLM(rasterConfig->viewDir);
    glm::vec3 viewUp = vec3ToGLM(rasterConfig->viewUp);
    //glm::vec3 center = vec3ToGLM(aabb.center();
    //mesh->need_bsphere();
    trimesh::vec3 tcenter = aabb.min + (aabb.max - aabb.min) * 0.5f;
    glm::vec3 center = glm::vec3(tcenter.x, tcenter.y, tcenter.z);

    glm::vec3 position = center - viewDir * d;

    glm::highp_mat4 viewMatrix = glm::lookAt(position, center, viewUp);
    glm::highp_mat4 modelMatrix;
    glm::highp_mat4 modelViewProjectionMatrix = projectionMatrix * viewMatrix * modelMatrix;

    Vec3 viewPos = Vec3(position.x, position.y, position.z);
    //for (const Triangle& triangle : *mesh)
    //for (const trimesh::TriMesh::Face& triangle : mesh->faces)

    size_t size = triangles.size() / 3;
    for (size_t i = 0; i < size; ++i)
    {
        if ((3 * i + 2) >= triangles.size())
            break;

        // project vertices to screen coordinates
        //glm::vec3 v0 = glmMat4x4MulVec3(modelViewProjectionMatrix, vec3ToGLM(triangle.vertices[0]));
        //glm::vec3 v1 = glmMat4x4MulVec3(modelViewProjectionMatrix, vec3ToGLM(triangle.vertices[1]));
        //glm::vec3 v2 = glmMat4x4MulVec3(modelViewProjectionMatrix, vec3ToGLM(triangle.vertices[2]));
        const trimesh::point& p0 = triangles.at(3 * i);
        const trimesh::point& p1 = triangles.at(3 * i + 1);
        const trimesh::point& p2 = triangles.at(3 * i + 2);
        glm::vec3 v0 = glmMat4x4MulVec3(modelViewProjectionMatrix, glm::vec3(p0.x, p0.y, p0.z));
        glm::vec3 v1 = glmMat4x4MulVec3(modelViewProjectionMatrix, glm::vec3(p1.x, p1.y, p1.z));
        glm::vec3 v2 = glmMat4x4MulVec3(modelViewProjectionMatrix, glm::vec3(p2.x, p2.y, p2.z));

        // triangle bounding box
        float min_x = std::min(v0.x, std::min(v1.x, v2.x));
        float min_y = std::min(v0.y, std::min(v1.y, v2.y));
        float max_x = std::max(v0.x, std::max(v1.x, v2.x));
        float max_y = std::max(v0.y, std::max(v1.y, v2.y));

        // bounding box in screen space
        unsigned smin_x = static_cast<unsigned>(std::max(0, static_cast<int>((min_x + 1.0f) / 2.0f * width)));
        unsigned smin_y = static_cast<unsigned>(std::max(0, static_cast<int>((min_y + 1.0f) / 2.0f * height)));
        unsigned smax_x = static_cast<unsigned>(std::max(0, std::min(int(width - 1), static_cast<int>((max_x + 1.0f) / 2.0f * width))));
        unsigned smax_y = static_cast<unsigned>(std::max(0, std::min(int(height - 1), static_cast<int>((max_y + 1.0f) / 2.0f * height))));

        for (unsigned y = smin_y; y <= smax_y; ++y)
        {
            for (unsigned x = smin_x; x <= smax_x; ++x)
            {
                // normalize screen coords [-1,1]
                const float nx = 2.0f * (x / static_cast<float>(width) - 0.5f);
                const float ny = 2.0f * (y / static_cast<float>(height) - 0.5f);

                glm::vec2 P = glm::vec2{ nx, ny };
                glm::vec2 V0 = glm::vec2(v0);
                glm::vec2 V1 = glm::vec2(v1);
                glm::vec2 V2 = glm::vec2(v2);

#if SWAP_STL_01
                bool inside = true;
                float f = edgeFunction(P, V0, V1);
                inside &= (f <= FLT_EPSILON); // ¼´f <= 0.0f
                f = edgeFunction(P, V1, V2);
                inside &= (f <= FLT_EPSILON);
                f = edgeFunction(P, V2, V0);
                inside &= (f <= FLT_EPSILON);

                if (!inside)
                {
                    continue;
                }

                // calculate baricentric coords
                float area = edgeFunction(V0, V1, V2);
                float w0 = edgeFunction(V1, V2, P) / area;
                float w1 = edgeFunction(V2, V0, P) / area;
                float w2 = edgeFunction(V0, V1, P) / area;
#else
                bool inside = true;
                float f = edgeFunction(P, V0, V1);
                inside &= (f >= -FLT_EPSILON); // ¼´f >= 0.0f
                f = edgeFunction(P, V1, V2);
                inside &= (f >= -FLT_EPSILON);
                f = edgeFunction(P, V2, V0);
                inside &= (f >= -FLT_EPSILON);

                if (!inside)
                {
                    continue;
                }

                // calculate baricentric coords
                float area = abs(edgeFunction(V0, V1, V2));
                float w0 = abs(edgeFunction(V1, V2, P)) / area;
                float w1 = abs(edgeFunction(V2, V0, P)) / area;
                float w2 = abs(edgeFunction(V0, V1, P)) / area;
#endif
                // the z position at point p by interpolating the z position of all 3 vertices
                float pz = w0 * v0.z + w1 * v1.z + w2 * v2.z;
                float px = w0 * v0.x + w1 * v1.x + w2 * v2.x;
                float py = w0 * v0.y + w1 * v1.y + w2 * v2.y;

                if (!zbuffer.testAndSet(x, y, pz))
                {
                    continue;
                }

                // calculate lightning
                // diffuse
                Vec3 s2l = rasterConfig->lightPos - Vec3{ px, py, pz };
                //Vec3 diffuse_color = std::max(0.0f, dot(triangle.normal, s2l)) * rasterConfig->diffuseColor;
                
                trimesh::vec3 sn = trimesh::normalized(trimesh::trinorm(p0,p1, p2));
                Vec3 vsn = Vec3(sn.x, sn.y, sn.z);
                Vec3 diffuse_color = std::max(0.0f, dot(vsn, s2l)) * rasterConfig->diffuseColor;

                // specular
                Vec3 frag_pos = { px, py, pz };
                Vec3 light_dir = (rasterConfig->lightPos - frag_pos).normalize();
                Vec3 view_dir = (viewPos - frag_pos).normalize();
                //Vec3 reflect_dir = reflect(-light_dir, triangle.normal);
                Vec3 reflect_dir = reflect(-light_dir, vsn);
                Vec3 specular_color = std::pow(std::max(dot(view_dir, reflect_dir), 0.0f), 1.0f) * rasterConfig->specularColor;

                // merge
                Vec3 color(1.0f, 0.0f, 0.0f);
                color = (rasterConfig->ambientColor + diffuse_color + specular_color) * rasterConfig->modelColor;

                // output pixel color
                picture->setRGB(x, y, color.x, color.y, color.z);
            }
        }
    }

    return true;
}

bool Raster::raster(Picture* picture, Mesh* mesh, RasterConfig* rasterConfig)
{
    if (!picture || !mesh || !rasterConfig)
    {
        return false;
    }

    size_t width = picture->width();
    size_t height = picture->height();

    if (width == 0 || height == 0)
    {
        return false;
    }

    ZBuffer zbuffer(width, height);
    zbuffer.setTestMethod(true);

    // generate AABB and find its center
    AABBox aabb = AABBox(*mesh);
    // create model view projection matrix
    float fovy = 30.0f * 3.1415926f / 180.0f;
    float len = aabb.size().length() / 2.0f;
    float d = len / sinf(fovy / 2.0f);
    float nearPlane = d - len;
    float farPlane = d + len;

    glm::highp_mat4 projectionMatrix = glm::perspective(fovy, (float)width / (float)height, nearPlane, farPlane);

    glm::vec3 viewDir = vec3ToGLM(rasterConfig->viewDir);
    glm::vec3 viewUp = vec3ToGLM(rasterConfig->viewUp);
    glm::vec3 center = vec3ToGLM(aabb.center());
    glm::vec3 position = center - viewDir * d;

    glm::highp_mat4 viewMatrix = glm::lookAt(position, center, viewUp);
    glm::highp_mat4 modelMatrix;
    glm::highp_mat4 modelViewProjectionMatrix = projectionMatrix * viewMatrix * modelMatrix;

    Vec3 viewPos = Vec3(position.x, position.y, position.z);
    for (const Triangle& triangle : *mesh)
    {
        // project vertices to screen coordinates
        glm::vec3 v0 = glmMat4x4MulVec3(modelViewProjectionMatrix, vec3ToGLM(triangle.vertices[0]));
        glm::vec3 v1 = glmMat4x4MulVec3(modelViewProjectionMatrix, vec3ToGLM(triangle.vertices[1]));
        glm::vec3 v2 = glmMat4x4MulVec3(modelViewProjectionMatrix, vec3ToGLM(triangle.vertices[2]));

        // triangle bounding box
        float min_x = std::min(v0.x, std::min(v1.x, v2.x));
        float min_y = std::min(v0.y, std::min(v1.y, v2.y));
        float max_x = std::max(v0.x, std::max(v1.x, v2.x));
        float max_y = std::max(v0.y, std::max(v1.y, v2.y));

        // bounding box in screen space
        unsigned smin_x = static_cast<unsigned>(std::max(0, static_cast<int>((min_x + 1.0f) / 2.0f * width)));
        unsigned smin_y = static_cast<unsigned>(std::max(0, static_cast<int>((min_y + 1.0f) / 2.0f * height)));
        unsigned smax_x = static_cast<unsigned>(std::max(0, std::min(int(width - 1), static_cast<int>((max_x + 1.0f) / 2.0f * width))));
        unsigned smax_y = static_cast<unsigned>(std::max(0, std::min(int(height - 1), static_cast<int>((max_y + 1.0f) / 2.0f * height))));

        for (unsigned y = smin_y; y <= smax_y; ++y)
        {
            for (unsigned x = smin_x; x <= smax_x; ++x)
            {
                // normalize screen coords [-1,1]
                const float nx = 2.0f * (x / static_cast<float>(width) - 0.5f);
                const float ny = 2.0f * (y / static_cast<float>(height) - 0.5f);

                glm::vec2 P = glm::vec2{ nx, ny };
                glm::vec2 V0 = glm::vec2(v0);
                glm::vec2 V1 = glm::vec2(v1);
                glm::vec2 V2 = glm::vec2(v2);

#if SWAP_STL_01
                bool inside = true;
                float f = edgeFunction(P, V0, V1);
                inside &= (f <= FLT_EPSILON); // ¼´f <= 0.0f
                f = edgeFunction(P, V1, V2);
                inside &= (f <= FLT_EPSILON);
                f = edgeFunction(P, V2, V0);
                inside &= (f <= FLT_EPSILON);

                if (!inside)
                {
                    continue;
                }

                // calculate baricentric coords
                float area = edgeFunction(V0, V1, V2);
                float w0 = edgeFunction(V1, V2, P) / area;
                float w1 = edgeFunction(V2, V0, P) / area;
                float w2 = edgeFunction(V0, V1, P) / area;
#else
                bool inside = true;
                float f = edgeFunction(P, V0, V1);
                inside &= (f >= -FLT_EPSILON); // ¼´f >= 0.0f
                f = edgeFunction(P, V1, V2);
                inside &= (f >= -FLT_EPSILON);
                f = edgeFunction(P, V2, V0);
                inside &= (f >= -FLT_EPSILON);

                if (!inside)
                {
                    continue;
                }

                // calculate baricentric coords
                float area = abs(edgeFunction(V0, V1, V2));
                float w0 = abs(edgeFunction(V1, V2, P)) / area;
                float w1 = abs(edgeFunction(V2, V0, P)) / area;
                float w2 = abs(edgeFunction(V0, V1, P)) / area;
#endif
                // the z position at point p by interpolating the z position of all 3 vertices
                float pz = w0 * v0.z + w1 * v1.z + w2 * v2.z;
                float px = w0 * v0.x + w1 * v1.x + w2 * v2.x;
                float py = w0 * v0.y + w1 * v1.y + w2 * v2.y;

                if (!zbuffer.testAndSet(x, y, pz))
                {
                    continue;
                }

                // calculate lightning
                // diffuse
                Vec3 s2l = rasterConfig->lightPos - Vec3{ px, py, pz };
                Vec3 diffuse_color = std::max(0.0f, dot(triangle.normal, s2l)) * rasterConfig->diffuseColor;

                // specular
                Vec3 frag_pos = { px, py, pz };
                Vec3 light_dir = (rasterConfig->lightPos - frag_pos).normalize();
                Vec3 view_dir = (viewPos - frag_pos).normalize();
                Vec3 reflect_dir = reflect(-light_dir, triangle.normal);
                Vec3 specular_color = std::pow(std::max(dot(view_dir, reflect_dir), 0.0f), 1.0f) * rasterConfig->specularColor;

                // merge
                Vec3 color(1.0f, 0.0f, 0.0f);
                color = (rasterConfig->ambientColor + diffuse_color + specular_color) * rasterConfig->modelColor;

                // output pixel color
                picture->setRGB(x, y, color.x, color.y, color.z);
            }
        }
    }

    return true;
}

bool Raster::raster(Picture* picture, TopoMesh* topoMesh, RasterConfig* rasterConfig)
{
    if (!picture || !topoMesh || !rasterConfig)
    {
        return false;
    }

    size_t width = picture->width();
    size_t height = picture->height();

    if (width == 0 || height == 0)
    {
        return false;
    }

    ZBuffer zbuffer(width, height);
    zbuffer.setTestMethod(true);

    // generate AABB and find its center
    AABBox aabb = topoMesh->box;
    // create model view projection matrix
    float fovy = 30.0f * 3.1415926f / 180.0f;
    float len = aabb.size().length() / 2.0f;
    float d = len / sinf(fovy / 2.0f);
    float nearPlane = d - len;
    float farPlane = d + len;

    glm::highp_mat4 projectionMatrix = glm::perspective(fovy, (float)width / (float)height, nearPlane, farPlane);

    glm::vec3 viewDir = vec3ToGLM(rasterConfig->viewDir);
    glm::vec3 viewUp = vec3ToGLM(rasterConfig->viewUp);
    glm::vec3 center = vec3ToGLM(aabb.center());
    glm::vec3 position = center - viewDir * d;

    glm::highp_mat4 viewMatrix = glm::lookAt(position, center, viewUp);
    glm::highp_mat4 modelMatrix;
    glm::highp_mat4 modelViewProjectionMatrix = projectionMatrix * viewMatrix * modelMatrix;

    Vec3 viewPos = Vec3(position.x, position.y, position.z);
    for (const Tri& tri : topoMesh->tris)
    {
        const Vertex& vertex0 = topoMesh->vertexes.at(tri.i[0]);
        const Vertex& vertex1 = topoMesh->vertexes.at(tri.i[1]);
        const Vertex& vertex2 = topoMesh->vertexes.at(tri.i[2]);

        // project vertices to screen coordinates
        glm::vec3 v0 = glmMat4x4MulVec3(modelViewProjectionMatrix, vec3ToGLM(vertex0.vertex));
        glm::vec3 v1 = glmMat4x4MulVec3(modelViewProjectionMatrix, vec3ToGLM(vertex1.vertex));
        glm::vec3 v2 = glmMat4x4MulVec3(modelViewProjectionMatrix, vec3ToGLM(vertex2.vertex));

        // triangle bounding box
        float min_x = std::min(v0.x, std::min(v1.x, v2.x));
        float min_y = std::min(v0.y, std::min(v1.y, v2.y));
        float max_x = std::max(v0.x, std::max(v1.x, v2.x));
        float max_y = std::max(v0.y, std::max(v1.y, v2.y));

        // bounding box in screen space
        unsigned smin_x = static_cast<unsigned>(std::max(0, static_cast<int>((min_x + 1.0f) / 2.0f * width)));
        unsigned smin_y = static_cast<unsigned>(std::max(0, static_cast<int>((min_y + 1.0f) / 2.0f * height)));
        unsigned smax_x = static_cast<unsigned>(std::max(0, std::min(int(width - 1), static_cast<int>((max_x + 1.0f) / 2.0f * width))));
        unsigned smax_y = static_cast<unsigned>(std::max(0, std::min(int(height - 1), static_cast<int>((max_y + 1.0f) / 2.0f * height))));

        for (unsigned y = smin_y; y <= smax_y; ++y)
        {
            for (unsigned x = smin_x; x <= smax_x; ++x)
            {
                // normalize screen coords [-1,1]
                const float nx = 2.0f * (x / static_cast<float>(width) - 0.5f);
                const float ny = 2.0f * (y / static_cast<float>(height) - 0.5f);

                glm::vec2 P = glm::vec2{ nx, ny };
                glm::vec2 V0 = glm::vec2(v0);
                glm::vec2 V1 = glm::vec2(v1);
                glm::vec2 V2 = glm::vec2(v2);

                bool inside = true;
                float f = edgeFunction(P, V0, V1);
                inside &= ((f >= -FLT_EPSILON && f <= FLT_EPSILON) || f > FLT_EPSILON); // ¼´f <= 0.0f
                f = edgeFunction(P, V1, V2);
                inside &= ((f >= -FLT_EPSILON && f <= FLT_EPSILON) || f > FLT_EPSILON);
                f = edgeFunction(P, V2, V0);
                inside &= ((f >= -FLT_EPSILON && f <= FLT_EPSILON) || f > FLT_EPSILON);

                if (!inside)
                {
                    continue;
                }

                // calculate baricentric coords
                float area = abs(edgeFunction(V0, V1, V2));
                float w0 = abs(edgeFunction(V1, V2, P)) / area;
                float w1 = abs(edgeFunction(V2, V0, P)) / area;
                float w2 = abs(edgeFunction(V0, V1, P)) / area;

                // the z position at point p by interpolating the z position of all 3 vertices
                float pz = w0 * v0.z + w1 * v1.z + w2 * v2.z;
                float px = w0 * v0.x + w1 * v1.x + w2 * v2.x;
                float py = w0 * v0.y + w1 * v1.y + w2 * v2.y;

                if (!zbuffer.testAndSet(x, y, pz))
                {
                    continue;
                }

                Vec3 normal;    //frag normal
                //normal = w0 * vertex0.normal + w1 * vertex1.normal + w2 * vertex2.normal;
                normal = vertex0.normal;
                normal.normalized();

                // calculate lightning
                // diffuse
                Vec3 s2l = rasterConfig->lightPos - Vec3{ px, py, pz };
                Vec3 diffuse_color = std::max(0.0f, dot(normal, s2l)) * rasterConfig->diffuseColor;

                // specular
                Vec3 frag_pos = { px, py, pz };
                Vec3 light_dir = (rasterConfig->lightPos - frag_pos).normalize();
                Vec3 view_dir = (viewPos - frag_pos).normalize();
                Vec3 reflect_dir = reflect(-light_dir, normal);
                Vec3 specular_color = std::pow(std::max(dot(view_dir, reflect_dir), 0.0f), 1.0f) * rasterConfig->specularColor;

                // merge
                Vec3 color(1.0f, 0.0f, 0.0f);
                color = (rasterConfig->ambientColor + diffuse_color + specular_color) * rasterConfig->modelColor;

                // output pixel color
                picture->setRGB(x, y, color.x, color.y, color.z);
            }
        }
    }

    return true;
}
