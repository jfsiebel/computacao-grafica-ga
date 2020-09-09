#include "Obj3D.h"

Obj3D::Obj3D(){};
Obj3D::~Obj3D(){};

bool Obj3D::getEliminavel()
{
    return this->eliminavel;
}

glm::vec3 Obj3D::getDirecao()
{
    return this->direcao;
}

Mesh *Obj3D::getMesh()
{
    return this->mesh;
}

int Obj3D::getStatus()
{
    return this->status;
}

glm::mat4 Obj3D::getTransform()
{
    return this->transform;
}

void Obj3D::setEliminavel(bool eliminavel)
{
    this->eliminavel = eliminavel;
}

void Obj3D::setDirecao(glm::vec3 direcao)
{
    this->direcao = direcao;
}

void Obj3D::setMesh(Mesh *mesh)
{
    this->mesh = mesh;
}

void Obj3D::setStatus(int status)
{
    this->status = status;
}

void Obj3D::setTransform(glm::mat4 transform)
{
    this->transform = transform;
}