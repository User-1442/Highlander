#include "glm/glm.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include "objloader.h"
#include "Scene.h"
#include <unordered_map>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

std::vector<glm::vec3> positions;
std::vector<unsigned int> indices;
std::vector<glm::vec2> uvs;
std::vector<glm::vec3> normals;

Assimp::Importer objimporter;
Assimp::Importer fbximporter;

struct Vertex {
    glm::vec3 position;
    glm::vec2 uv;
    glm::vec3 normal;
    glm::vec3 vtangents;

    bool operator==(const Vertex other) const {
        return position == other.position &&
            uv == other.uv &&
            normal == other.normal;
    };
};

struct VertexHasher {
    size_t operator()(const Vertex& v) const {
        size_t h = 0;

        auto hash_combine = [&](size_t& seed, size_t val) {
            seed ^= val + 0x9e37799b9 + (seed << 6);
        };

        hash_combine(h, std::hash<float>{}(v.position.x));
        hash_combine(h, std::hash<float>{}(v.position.y));
        hash_combine(h, std::hash<float>{}(v.position.z));
        hash_combine(h, std::hash<float>{}(v.uv.x));
        hash_combine(h, std::hash<float>{}(v.uv.y));
        hash_combine(h, std::hash<float>{}(v.normal.x));
        hash_combine(h, std::hash<float>{}(v.normal.y));
        hash_combine(h, std::hash<float>{}(v.normal.z));

        return h;
    }
};

void parceIndices(const std::string& indice, unsigned int& pos, unsigned int& uv_index, unsigned int& normal_index) { //what is the singular if indices??
    std::stringstream ss(indice);
    std::string numberletter; //stores the number as a letter
    int currentthingy = 0;

    while (std::getline(ss, numberletter, '/')) {
        if (!numberletter.empty()) {
            unsigned int currentnumber = std::stoul(numberletter);
            if (currentthingy == 0) {
                pos = currentnumber;
            }
            if (currentthingy == 1) {
                uv_index = currentnumber;
            }
            if (currentthingy == 2) {
                normal_index = currentnumber;
            }
        currentthingy++;
        }
    }
}

int objload(
    const char* filename,
    std::vector<glm::vec3>& out_positions,
    std::vector<unsigned int>& out_indices,
    std::vector<glm::vec2>& out_uvs,
    std::vector<glm::vec3>& out_normals,
    std::vector<glm::vec3>& out_tangents
) {

    const aiScene* scene = objimporter.ReadFile(filename,
    aiProcess_Triangulate |
    aiProcess_CalcTangentSpace |
    aiProcess_JoinIdenticalVertices |
    aiProcess_FlipUVs);//makes sure textures aren't spidey boi flipped upside down

    Vertex Mesh;

    unsigned int vertexOffset = 0;

    for (unsigned int m = 0; m < scene->mNumMeshes; m++) {
        aiMesh* mesh = scene->mMeshes[m];

        for (int i = 0; i < mesh->mNumVertices; i++) {
            //positions
            Mesh.position.x = mesh->mVertices[i].x;
            Mesh.position.y = mesh->mVertices[i].y;
            Mesh.position.z = mesh->mVertices[i].z;
            out_positions.push_back(Mesh.position);

            if (mesh->mTextureCoords[0]) {
                Mesh.uv.x = mesh->mTextureCoords[0][i].x;
                Mesh.uv.y = mesh->mTextureCoords[0][i].y;
                out_uvs.push_back(Mesh.uv);
            } else {
                out_uvs.push_back(glm::vec2(0.0f, 0.0f));
            }

            Mesh.normal.x = mesh->mNormals[i].x;
            Mesh.normal.y = mesh->mNormals[i].y;
            Mesh.normal.z = mesh->mNormals[i].z;
            out_normals.push_back(Mesh.normal);

            Mesh.vtangents.x = mesh->mTangents[i].x;
            Mesh.vtangents.y = mesh->mTangents[i].y;
            Mesh.vtangents.z = mesh->mTangents[i].z;
            out_tangents.push_back(Mesh.vtangents);
        }

        for (int i = 0; i < mesh->mNumFaces; i++) {
            aiFace& face = mesh->mFaces[i];
            for (int j = 0; j < 3; j++) {
                out_indices.push_back(face.mIndices[j] + vertexOffset);
            }
        }
        vertexOffset += mesh->mNumVertices;

    }

    return out_indices.size();
}

void ProcessNode(const aiScene* scene, aiNode* Node) {
    std::cout << "Processing Node For FBX File\n";
    for (int i = 0; i < Node->mNumMeshes; i++) {
        Vertex Mesh;
        Object obj;
        unsigned int meshIndex = Node->mMeshes[i];
        aiMesh* mesh = scene->mMeshes[meshIndex];
        for (int j = 0; j < mesh->mNumVertices; j++) {
            Mesh.position.x = mesh->mVertices[j].x;
            Mesh.position.y = mesh->mVertices[j].y;
            Mesh.position.z = mesh->mVertices[j].z;
            obj.vertices.push_back(Mesh.position);
            Mesh.normal.x = mesh->mNormals[j].x;
            Mesh.normal.y = mesh->mNormals[j].y;
            Mesh.normal.z = mesh->mNormals[j].z;
            obj.normals.push_back(Mesh.normal);
            Mesh.vtangents.x = mesh->mTangents[j].x;
            Mesh.vtangents.y = mesh->mTangents[j].y;
            Mesh.vtangents.z = mesh->mTangents[j].z;
            obj.objtangents.push_back(Mesh.vtangents);
        }
        Objects.push_back(obj);
    }
    for (int i = 0; i < Node->mNumChildren; i++) {
        ProcessNode(scene, Node->mChildren[i]);
    }
}

int fbxload(const char* filename) {
    std::cout << filename << std::endl;
    const aiScene* scene = fbximporter.ReadFile(filename,
        aiProcess_CalcTangentSpace |
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_FlipUVs |
        aiProcess_MakeLeftHanded);
    if (scene == nullptr) {
        std::cout << "ERROR::COULDN'T::LOAD::FILE\n";
        return 0;
    }
    ProcessNode(scene, scene->mRootNode);
    return 1;
}
