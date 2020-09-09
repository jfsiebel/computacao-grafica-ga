#ifndef COMPUTACAO_GRAFICA_GA_MESH_H
#define COMPUTACAO_GRAFICA_GA_MESH_H

#include <string>
#include <vector>

#include "glm/vec2.hpp"
#include "glm/vec3.hpp"

#include "Group.h"

class Mesh
{
private:
    std::vector<Group *> groups;

    glm::vec3 max;
    glm::vec3 min;
    std::string mtllib;

    std::vector<glm::vec3 *> normals;
    std::vector<glm::vec2 *> mappings;
    std::vector<glm::vec3 *> vertices;

public:
    Mesh();
    ~Mesh();

    std::vector<Group *> getGroups();
    glm::vec3 getMax();
    glm::vec3 getMin();
    std::string getMtllib();

    std::vector<glm::vec3 *> getNormals();
    std::vector<glm::vec2 *> getMappings();
    std::vector<glm::vec3 *> getVertices();

    void addGroup(Group *group);

    void setGroups(std::vector<Group *> groups);
    void setMax(glm::vec3 max);
    void setMin(glm::vec3 min);
    void setMtllib(std::string mtllib);

    void addNormal(glm::vec3 *normal);
    void addMapping(glm::vec2 *mapping);
    void addVertex(glm::vec3 *vertex);

    void setNormals(std::vector<glm::vec3 *> normals);
    void setMappings(std::vector<glm::vec2 *> mappings);
    void setVertices(std::vector<glm::vec3 *> vertices);
};

#endif //COMPUTACAO_GRAFICA_GA_MESH_H
