#include "Group.h"

Group::Group() {
    this->numOfVertices = 0;
    this->VAO = 0;
}
Group::~Group() {}

std::vector<Face *> Group::getFaces()
{
    return this->faces;
}

std::string Group::getMaterial()
{
    return this->material;
}

std::string Group::getName()
{
    return this->name;
}

GLuint Group::getVAO()
{
    return this->VAO;
}

int Group::getNumOfVertices()
{
    return this->numOfVertices;
}

void Group::addFace(Face *face)
{
    this->faces.push_back(face);
    this->numOfVertices += face->getVerts().size();
}

void Group::setFaces(std::vector<Face *> faces)
{
    this->faces = faces;
}

void Group::setMaterial(std::string material)
{
    this->material = material;
}

void Group::setName(std::string name)
{
    this->name = name;
}

void Group::setVAO(GLuint VAO)
{
    this->VAO = VAO;
}