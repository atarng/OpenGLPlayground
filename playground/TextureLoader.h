#pragma once
#include <vector>
#include <glm/glm.hpp> // Include GLM (MATH)

#include "IGeometry.h"
#include "IFileReader.h"


using namespace std;
using namespace glm;
using namespace atrng;

class TextureLoader : public IFileReader
{
public:
    vector<int> LoadFiles(vector<string>);
    int LoadBmpAsNormalMap(string);

private:

};
