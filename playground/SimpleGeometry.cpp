#include "SimpleGeometry.h"

SimpleGeometry::SimpleGeometry()
{
    m_vertices.push_back(vec3(-1.0f,-1.0f, 0.0f));
    m_vertices.push_back(vec3( 1.0f,-1.0f, 0.0f));
    m_vertices.push_back(vec3( 0.0f, 1.0f, 0.0f));
}

vector<vec3> SimpleGeometry::GetVertices()
{
    return m_vertices;
}