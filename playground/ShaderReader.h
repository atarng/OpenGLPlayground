#pragma once

#include "IFileReader.h"
#include <string>

using namespace atrng;
using namespace std;

class ShaderReader : public IFileReader
{
public:
    int LoadFiles(vector<string>);
    int LoadShaders(string, string);

    //int LoadShaders_c(const char *, const char *);
protected:
private:

};