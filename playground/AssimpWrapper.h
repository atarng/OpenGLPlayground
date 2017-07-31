#pragma once

#include <iostream>
#include <string>

#include "assimp/Importer.hpp"
#include "assimp/cimport.h"
#include "assimp/postprocess.h"

//3rd party
#include "common_header.h"
#include "vertexBufferObject.h"

#include "IFileReader.h"

using namespace Assimp;
using namespace std;
using namespace atrng;

#define NUM_BONES_PER_VERTEX 4

class AssimpWrapper : IFileReader {
public:
    struct VertexBoneData
    {
        unsigned int IDs[NUM_BONES_PER_VERTEX];
        float Weights[NUM_BONES_PER_VERTEX];
    };

    vector<int> LoadFiles(vector<string>);
    bool ImportFileName(string);

    void FinalizeVBO();
    void Render();

    //void LoadBones(unsigned int MeshIndex, const aiMesh* pMesh, vector<VertexBoneData>& Bones)

protected:
    bool ConvertAIScene_OpenGl(const aiScene*);
private:
    Importer m_importer;

    vector<vec3> m_vertices;
    vector<vec2> m_uvs;
    vector<vec3> m_normals;
    GLuint vertexbuffer, uvbuffer, normalbuffer;

/////////
    bool bLoaded;
    CVertexBufferObject vboModelData;
    //static vector<CTexture> tTextures;
    vector<int> iMeshStartIndices;
    vector<int> iMeshSizes;
    vector<int> iMaterialIndices;
    int iNumMaterials;
/////////

};