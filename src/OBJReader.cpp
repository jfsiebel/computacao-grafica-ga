#include "OBJReader.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include "glm/glm.hpp"

using namespace std;

OBJReader::OBJReader() {};

OBJReader::~OBJReader() {};

vector<Material *> OBJReader::readMaterials(std::string filePath) {
    vector<Material *> materials;
    Material *material;

    ifstream arq(filePath);

    while (!arq.eof()) {
        string line;

        getline(arq, line);

        stringstream ss;

        ss << line;

        std::string instruction;

        ss >> instruction;

        switch (instruction) {
            case "newmtl":
                material = new Material();
                materials.push_back(material);
                std::string id;
                ss >> id;
                material->setId(id);
                break;
            case "illum":
                break;
            case "Kd":
                float kdR, kdG, kdB;
                ss >> kdR >> kdG >> kdB;
                material->setKd(glm::vec3(kdR, kdG, kdB));
                break;
            case "Ka":
                float kaR, kaG, kaB;
                ss >> kaR >> kaG >> kaB;
                material->setKa(glm::vec3(kaR, kaG, kaB));
                break;
            case "Ks":
                float ksR, ksG, ksB;
                ss >> ksR >> ksG >> ksB;
                material->setKs(glm::vec3(ksR, ksG, ksB));
                break;
            case "Tf":
                break;
            case "map_Kd":
                string mapKd;
                ss >> mapKd;
                material->setMapKd(mapKd);
                break;
            case "Ns":
                float ns;
                ss >> ns;
                material->setNs(ns);
                break;
            default:
                continue;
    }
    arq.close();
    return materials;
}

Mesh *OBJReader::read(std::string filename) {
    Mesh *mesh = new Mesh();
    Group *group = new Group();

    ifstream arq(filename);

    string currentGroup;
    group->setName("default");

    bool firstVertex = true;

    while (!arq.eof()) {
        string line;

        getline(arq, line);

        stringstream ss;

        ss << line;

        string instruction;

        ss >> instruction;

        switch (instruction) {
            case "mtllib":
                string mtllib;
                ss >> mtllib;
                mesh->setMtllib(mtllib);
                break;
            case "g":
                string groupName;
                ss >> groupName;

                if (!currentGroup.empty() && currentGroup != groupName) {
                    mesh->addGroup(group);
                    group = new Group();
                }

                currentGroup = groupName;
                group->setName(groupName);
                break;
            case "usemtl":
                string material;
                ss >> material;
                group->setMaterial(material);
                break;
            case "v":
                float x, y, z;
                ss >> x >> y >> z;

                if (firstVertex) {
                    mesh->setMin(glm::vec3(x, y, z));
                    mesh->setMax(glm::vec3(x, y, z));
                    firstVertex = false;
                } else {
                    glm::vec3 min = mesh->getMin();
                    glm::vec3 max = mesh->getMax();

                    if (x < min.x) min = glm::vec3(x, min.y, min.z);
                    if (y < min.y) min = glm::vec3(min.x, y, min.z);
                    if (z < min.z) min = glm::vec3(min.x, min.y, z);

                    if (x > max.x) max = glm::vec3(x, max.y, max.z);
                    if (y > max.y) max = glm::vec3(max.x, y, max.z);
                    if (z > max.z) max = glm::vec3(max.x, max.y, z);

                    mesh->setMin(min);
                    mesh->setMax(max);
                }

                mesh->addVertex(new glm::vec3(x, y, z));
                break;
            case "vt":
                float x, y;
                ss >> x >> y;
                mesh->addMapping(new glm::vec2(x, y));
                break;
            case "vn":
                float x, y, z;
                ss >> x >> y >> z;
                mesh->addNormal(new glm::vec3(x, y, z));
                break;
            case "f":
                Face *f = new Face();
                int vertexQtd = 0;
                while (!ss.eof()) {
                    string token;
                    ss >> token;

                    if (token.empty()) continue;

                    stringstream ssToken;

                    ssToken << token;

                    vertexQtd++;
                    string vertex, texture, normal;

                    getline(ssToken, vertex, '/');
                    getline(ssToken, texture, '/');
                    getline(ssToken, normal, '/');

                    int i = stoi(vertex) - 1;

                    f->addVert(i);

                    if (!texture.empty()) {
                        int i = stoi(texture) - 1;
                        f->addText(i);
                    }

                    if (!normal.empty()) {
                        int i = stoi(normal) - 1;
                        f->addNorm(i);
                    }
                }

                // Obj possui um retângulo
                if (vertexQtd > 3) {
    //                1 ---- 2
    //                |      |
    //                4 ---- 3
    //
    //                1->2->3  1->3->4

                    vector<int> verts = f->getVerts();
                    vector<int> textures = f->getTexts();
                    vector<int> normals = f->getNorms();

                    //Quebra um retângulo em 2 triângulos
                    Face *f1 = new Face();

                    f1->addVert(verts[0]);
                    f1->addVert(verts[1]);
                    f1->addVert(verts[2]);

                    Face *f2 = new Face();
                    f2->addVert(verts[0]);
                    f2->addVert(verts[2]);
                    f2->addVert(verts[3]);

                    if (textures.size()) {
                        f1->addText(textures[0]);
                        f1->addText(textures[1]);
                        f1->addText(textures[2]);

                        f2->addText(textures[0]);
                        f2->addText(textures[2]);
                        f2->addText(textures[3]);
                    }

                    if (normals.size()) {
                        f1->addNorm(normals[0]);
                        f1->addNorm(normals[1]);
                        f1->addNorm(normals[2]);

                        f2->addNorm(normals[0]);
                        f2->addNorm(normals[2]);
                        f2->addNorm(normals[3]);
                    }

                    group->addFace(f1);
                    group->addFace(f2);
                } else {
                    group->addFace(f);
                }
                break;
            default:
                continue;
        }
    }
    arq.close();
    mesh->addGroup(group);

    return mesh;
}