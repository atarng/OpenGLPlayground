#pragma once

#include <glm/glm.hpp> // Include GLM (MATH)
using namespace glm;

namespace atrng{
class ICamera {
public:
    virtual mat4 GetProjectionMatrix() = 0;
    virtual mat4 GetViewMatrix() = 0;
};
}