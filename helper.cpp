#include "helper.h"

glm::vec3 vec3ToGLM(const Vec3& v)
{
    return { v.x, v.y, v.z };
}

glm::vec3 glmMat4x4MulVec3(const glm::mat4x4& mat, const glm::vec3& v)
{
    glm::vec4 vp = mat * glm::vec4(v.x, v.y, v.z, 1.0f);
    return glm::vec3(vp.x / vp.w, vp.y / vp.w, vp.z / vp.w);
}

float edgeFunction(const glm::vec2& p, const glm::vec2& a, const glm::vec2& b)
{
    return (p.x - a.x) * (b.y - a.y) - (p.y - a.y) * (b.x - a.x);
}
