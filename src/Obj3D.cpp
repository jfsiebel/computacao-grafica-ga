#include "Obj3D.h"

Obj3D::Obj3D() {
    this->eliminavel = false;
    this->isVisible = true;
    this->transform = glm::mat4(1.0f);
    this->direcao = glm::vec3(1.0f);
};

Obj3D::~Obj3D() {};

bool Obj3D::getEliminavel() {
    return this->eliminavel;
}

glm::vec3 Obj3D::getDirecao() {
    return this->direcao;
}

bool Obj3D::getIsVisible() {
    return this->isVisible;
}

Mesh *Obj3D::getMesh() {
    return this->mesh;
}

int Obj3D::getStatus() {
    return this->status;
}

glm::mat4 Obj3D::getTransform() {
    return this->transform;
}

void Obj3D::setEliminavel(bool eliminavel) {
    this->eliminavel = eliminavel;
}

void Obj3D::setDirecao(glm::vec3 direcao) {
    this->direcao = direcao;
}

void Obj3D::setIsVisible(bool isVisible) {
    this->isVisible = isVisible;
}

void Obj3D::setMesh(Mesh *mesh) {
    this->mesh = mesh;
}

void Obj3D::setStatus(int status) {
    this->status = status;
}

void Obj3D::setTransform(glm::mat4 transform) {
    this->transform = transform;
}