#pragma once

#include "ICamera.h"
#include <glm/glm.hpp> // Include GLM (MATH)

#include "IGeometry.h"

using namespace glm;
using namespace atrng;

class AttachedCamera : public ICamera
{
public:
    inline mat4 GetProjectionMatrix() { return m_projectionMatrix; }
    inline mat4 GetViewMatrix() { return m_viewMatrix; }
protected:
private:
    mat4 m_projectionMatrix;
    mat4 m_viewMatrix;

    IGeometry* m_attachedObject;
};