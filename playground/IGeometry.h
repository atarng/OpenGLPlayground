#pragma once
#include <vector>
#include <glm/glm.hpp> // Include GLM (MATH)

using namespace std;
using namespace glm;

namespace atrng
{
class IGeometry {
public:
    virtual vector<vec3> GetVertices() = 0;
};
}