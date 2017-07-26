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
    //bool ReadFile(const char *);
    vector<int> LoadFiles(vector<string>);

    inline vector<vec3> GetVertices() { return m_vertices; }
    inline vector<vec2> GetUVs() { return m_uvs; }
    inline vector<vec3> GetNormals() { return m_normals; }

private:
    vector<vec3> m_vertices;
    vector<vec2> m_uvs;
    vector<vec3> m_normals;
};
