#include "Mesh.h"

Mesh::Mesh() {}

Mesh::~Mesh() {}

std::vector<Group *> Mesh::getGroups() {
    return this->groups;
}

glm::vec3 Mesh::getMax() {
    return this->max;
}

glm::vec3 Mesh::getMin() {
    return this->min;
}

std::string Mesh::getMtllib() {
    return this->mtllib;
}

std::vector<glm::vec3 *> Mesh::getNormals() {
    return this->normals;
}

std::vector<glm::vec2 *> Mesh::getMappings() {
    return this->mappings;
}

std::vector<glm::vec3 *> Mesh::getVertices() {
    return this->vertices;
}

void Mesh::addGroup(Group *group) {
    this->groups.push_back(group);
}

void Mesh::setGroups(std::vector<Group *> groups) {
    this->groups = groups;
}

void Mesh::setMax(glm::vec3 max) {
    this->max = max;
}

void Mesh::setMin(glm::vec3 min) {
    this->min = min;
}

void Mesh::setMtllib(std::string mtllib) {
    this->mtllib = mtllib;
}

void Mesh::addNormal(glm::vec3 *normal) {
    this->normals.push_back(normal);
}

void Mesh::addMapping(glm::vec2 *mapping) {
    this->mappings.push_back(mapping);
}

void Mesh::addVertex(glm::vec3 *vertex) {
    this->vertices.push_back(vertex);
}

void Mesh::setNormals(std::vector<glm::vec3 *> normals) {
    this->normals = normals;
}

void Mesh::setMappings(std::vector<glm::vec2 *> mappings) {
    this->mappings = mappings;
}

void Mesh::setVertices(std::vector<glm::vec3 *> vertices) {
    this->vertices = vertices;
}