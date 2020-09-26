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

        if (instruction == "#" || instruction.empty()) {
            continue;
        } else if (instruction == "newmtl") {
            material = new Material();
            materials.push_back(material);
            std::string id;
            ss >> id;
            material->setId(id);
        } else if (instruction == "illum") {

        } else if (instruction == "Kd") {
            float kdR, kdG, kdB;
            ss >> kdR >> kdG >> kdB;
            material->setKd(glm::vec3(kdR, kdG, kdB));
        } else if (instruction == "Ka") {
            float kaR, kaG, kaB;
            ss >> kaR >> kaG >> kaB;
            material->setKa(glm::vec3(kaR, kaG, kaB));
        } else if (instruction == "Ks") {
            float ksR, ksG, ksB;
            ss >> ksR >> ksG >> ksB;
            material->setKs(glm::vec3(ksR, ksG, ksB));
        } else if (instruction == "Tf") {

        } else if (instruction == "map_Kd") {
            string mapKd;
            ss >> mapKd;
            material->setMapKd(mapKd);
        } else if (instruction == "Ns") {
            float ns;
            ss >> ns;
            material->setNs(ns);
        }
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

        if (instruction == "#" || instruction.empty()) {
            continue;
        } else if (instruction == "mtllib") {
            string mtllib;
            ss >> mtllib;
            mesh->setMtllib(mtllib);
        } else if (instruction == "g") {
            string groupName;
            ss >> groupName;

            if (!currentGroup.empty() && currentGroup != groupName) {
                mesh->addGroup(group);
                group = new Group();
            }

            currentGroup = groupName;
            group->setName(groupName);
        } else if (instruction == "usemtl") {
            string material;
            ss >> material;
            group->setMaterial(material);
        } else if (instruction == "v") {
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
        } else if (instruction == "vt") {
            float x, y;
            ss >> x >> y;
            mesh->addMapping(new glm::vec2(x, y));
        } else if (instruction == "vn") {
            float x, y, z;
            ss >> x >> y >> z;
            mesh->addNormal(new glm::vec3(x, y, z));
        } else if (instruction == "f") {
            Face *f = new Face();

            while (!ss.eof()) {
                string token;
                ss >> token;

                if (token.empty()) continue;

                stringstream ssToken;

                ssToken << token;

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
            group->addFace(f);
        }
    }
    arq.close();
    mesh->addGroup(group);

    return mesh;
}