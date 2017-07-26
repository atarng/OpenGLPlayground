#pragma once
#include <vector>
#include <glm/glm.hpp> // Include GLM (MATH)

#include "IFileReader.h"

using namespace std;
using namespace glm;
using namespace atrng;

class Font2dLoader : public IFileReader
{
public:
    //bool ReadFile(const char *);
    vector<int> LoadFiles(vector<string>);

    void printText2D(const char * text, int x, int y, int size);
    void cleanupText2D();

private:
    unsigned int Text2DTextureID, Text2DVertexBufferID, Text2DUVBufferID,
                 Text2DShaderID, Text2DUniformID;
};
