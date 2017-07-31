#include "AssimpWrapper.h"


#include "assimp/scene.h"
#include <assimp/DefaultLogger.hpp>

vector<int> AssimpWrapper::LoadFiles(vector<string> vs) {
    vector<int> ret_vector;

    string file_path_candidates[] = { "", "Assets/" };
    int size_of_file_paths = sizeof(file_path_candidates) / sizeof(file_path_candidates[0]);

    FILE * file = NULL;
    int f_i = 0;
    for (int i = 0; i < vs.size(); i++) {
        string filepath = "";
        while (file == NULL && f_i < size_of_file_paths) {
            filepath = (file_path_candidates[f_i] + vs[i]);
            file     = fopen(filepath.c_str(), "r");
            f_i++;
        }
        if (file == NULL) {
            getchar(); // clear out null character?
            return ret_vector;
        }
        if (ImportFileName(filepath)) {
            ret_vector.push_back(1);
        }
    }
    return ret_vector;
}

bool AssimpWrapper::ImportFileName(string assimp_file_name) {
    // Create a logger instance 
    DefaultLogger::create("AssimpLog.txt", Logger::VERBOSE);
    // Now I am ready for logging my stuff
    DefaultLogger::get()->info("this is my info-call");

    if (vboModelData.GetBufferID() == 0)
    {
        vboModelData.CreateVBO();
        //tTextures.reserve(50);
    }

    //aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType
    aiPostProcessSteps aipps = aiPostProcessSteps(aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);
    const aiScene* scene = m_importer.ReadFile(assimp_file_name, aipps);

    if (!scene) {
        cerr << "Assimp Failed to import File: " << assimp_file_name << endl;
        return false;
    }
    bool ret = ConvertAIScene_OpenGl(scene);



    // Kill it after the work is done
    DefaultLogger::kill();
    return ret;
}

bool AssimpWrapper::ConvertAIScene_OpenGl(const aiScene* scene) {
    const int iVertexTotalSize = sizeof(aiVector3D) * 2 + sizeof(aiVector2D);
    int iTotalVertices = 0;

    cout << "scene->mNumMeshes: " << scene->mNumMeshes << endl;
    for (int i = 0; i < scene->mNumMeshes; i++) {

        aiMesh* mesh = scene->mMeshes[i];
        int iMeshFaces = mesh->mNumFaces;
        cout << "mesh->mNumFaces: " << mesh->mNumFaces << endl;

        iMaterialIndices.push_back(mesh->mMaterialIndex);
        int iSizeBefore = vboModelData.GetCurrentSize();
        iMeshStartIndices.push_back(iSizeBefore / iVertexTotalSize);
        for(int j = 0; j < iMeshFaces; j++) {

            const aiFace& face = mesh->mFaces[j];

            // Triangulated
            for (int k = 0; k < 3; k++) {
                aiVector3D pos    = mesh->mVertices[face.mIndices[k]];
                aiVector3D uv     = mesh->mTextureCoords[0][face.mIndices[k]];
                aiVector3D normal = mesh->HasNormals() ? mesh->mNormals[face.mIndices[k]] : aiVector3D(1.0f, 1.0f, 1.0f);

                m_vertices.push_back(vec3(pos.x, pos.y, pos.z));
                m_uvs.push_back(vec2(uv.x, uv.y));
                m_normals.push_back(vec3(normal.x, normal.y, normal.z));

                ///*
                vboModelData.AddData(&pos,    sizeof(aiVector3D));
                vboModelData.AddData(&uv,     sizeof(aiVector2D));
                vboModelData.AddData(&normal, sizeof(aiVector3D));
                //*/
            }
        }
        int iMeshVertices = mesh->mNumVertices;
        iTotalVertices   += iMeshVertices;
        iMeshSizes.push_back((vboModelData.GetCurrentSize() - iSizeBefore) / iVertexTotalSize);
    }

    /* Ignore this part for now, we find the way they do materials/textures to be strange.
    iNumMaterials = scene->mNumMaterials;
    vector<int> materialRemap(iNumMaterials);
    for(int i = 0; i < iNumMaterials; i++) {
        const aiMaterial* material = scene->mMaterials[i];
        int a = 5;
        int texIndex = 0;
        aiString path;  // filename
        if (material->GetTexture(aiTextureType_DIFFUSE, texIndex, &path) == AI_SUCCESS) {
            string sDir         = GetDirectoryPath(sFilePath);
            string sTextureName = path.data;
            string sFullPath    = sDir + sTextureName;
            int iTexFound = -1;
            for (int j = 0; j < tTextures.size(); j++) {
                if (sFullPath == tTextures[j].GetPath()) {
                    iTexFound = j;
                    break;
                }
            }
            if (iTexFound != -1)materialRemap[i] = iTexFound;
            else
            {
                CTexture tNew;
                tNew.LoadTexture2D(sFullPath, true);
                materialRemap[i] = ESZ(tTextures);
                tTextures.push_back(tNew);
            }
        }
    }
    cout << "iMeshSizes : " << iMeshSizes.size() << endl;
    for(int i = 0; i < iMeshSizes.size(); i++){
        int iOldIndex       = iMaterialIndices[i];
        iMaterialIndices[i] = materialRemap[iOldIndex];
    }
    */

    return (bLoaded = true);
}
/***
 * atarng: TODO: sanity check this to conform with our tutorial method.
 */
void AssimpWrapper::FinalizeVBO() {
    //vboModelData.BindVBO(GL_ARRAY_BUFFER);
    //vboModelData.UploadDataToGPU(GL_STATIC_DRAW);

    ///*
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(vec3), &m_vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, m_uvs.size() * sizeof(vec2), &m_uvs[0], GL_STATIC_DRAW);

    glGenBuffers(1, &normalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, m_normals.size() * sizeof(vec3), &m_normals[0], GL_STATIC_DRAW);
    //*/

}

void AssimpWrapper::Render() {
    /*
    int iNumMeshes = iMeshSizes.size();
    for(int i = 0; i < iNumMeshes; i++) {
        int iMatIndex = iMaterialIndices[i];
        glDrawArrays(GL_TRIANGLES, iMeshStartIndices[i], iMeshSizes[i]);
    }
    //*/
    ///*
    // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    // 2nd attribute buffer : UVs
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    // 3rd attribute buffer : normals
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    // Draw the triangle !
    glDrawArrays(GL_TRIANGLES, 0, m_vertices.size());

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    //*/
}