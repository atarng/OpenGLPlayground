#pragma once
#include <vector>
#include <glm/glm.hpp> // Include GLM (MATH)

#include "IGeometry.h"
#include "IFileReader.h"


using namespace std;
using namespace glm;
using namespace atrng;

class ObjReader : public IFileReader, public IGeometry
{
public:

private:
    vector<vec3> m_vertices;
    vector<vec2> m_uvs;
    vector<vec3> m_normals;
};
