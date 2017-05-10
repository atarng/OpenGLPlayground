#pragma once

#include "IGeometry.h"

#include <vector>
#include <glm/glm.hpp> // Include GLM (MATH)


using namespace atrng;
using namespace std;
using namespace glm;

class SimpleGeometry : public IGeometry
{
public:
    SimpleGeometry();

    vector<vec3> GetVertices();

protected:
private:
    vector<vec3> m_vertices;
    //std::vector<glm::vec2> & uvs,
    //std::vector<glm::vec3> & normals
};