#pragma once
#include <string>
#include <vector>
using namespace std;

namespace atrng
{
class IFileReader {
public:
    virtual int LoadFiles(vector<string>) = 0;
};
}