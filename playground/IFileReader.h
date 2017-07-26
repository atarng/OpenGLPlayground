#pragma once
#include <string>
#include <vector>
using namespace std;

namespace atrng
{
class IFileReader {
public:
    virtual vector<int> LoadFiles(vector<string>) = 0;
};
}