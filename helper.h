#ifndef _NULLSPACE_HELPER_1588997655352_H
#define _NULLSPACE_HELPER_1588997655352_H

#include <glm/glm.hpp>
#include "vec3.h"

glm::vec3 vec3ToGLM(const Vec3& v);
glm::vec3 glmMat4x4MulVec3(const glm::mat4x4& mat, const glm::vec3& v);
float edgeFunction(const glm::vec2& p, const glm::vec2& a, const glm::vec2& b);

#endif // _NULLSPACE_HELPER_1588997655352_H
