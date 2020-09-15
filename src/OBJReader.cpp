#include "OBJReader.h"

#include <fstream>
#include <iostream>
#include <sstream>

#include "glm/glm.hpp"

using namespace std;

OBJReader::OBJReader() {};

OBJReader::~OBJReader() {};

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

        stringstream sline;

        sline << line;

        string instruction;

        sline >> instruction;

        if (instruction == "#" || instruction.empty()) {
            continue;
        } else if (instruction == "mtllib") {
            string mtllib;
            sline >> mtllib;
            mesh->setMtllib(mtllib);
        } else if (instruction == "g") {
            string groupName;
            sline >> groupName;

            if (!currentGroup.empty() && currentGroup != groupName) {
                mesh->addGroup(group);
                group = new Group();
            }

            currentGroup = groupName;
            group->setName(groupName);
        } else if (instruction == "v") {
            float x, y, z;
            sline >> x >> y >> z;

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
            sline >> x >> y;
            mesh->addMapping(new glm::vec2(x, y));
        } else if (instruction == "vn") {
            float x, y, z;
            sline >> x >> y >> z;
            mesh->addNormal(new glm::vec3(x, y, z));
        } else if (instruction == "f") {
            Face *f = new Face();

            while (!sline.eof()) {
                string token;
                sline >> token;

                if (token.empty()) continue;

                stringstream stoken;

                stoken << token;

                string vertex, texture, normal;

                getline(stoken, vertex, '/');
                getline(stoken, texture, '/');
                getline(stoken, normal, '/');

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