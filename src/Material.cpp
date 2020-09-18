#include "Material.h"

Material::Material() {}

Material::~Material() {}

std::string Material::getId() {
    return this->id;
}

std::string Material::getMapKd() {
    return this->mapKd;
}

glm::vec3 Material::getKa() {
    return this->ka;
}

glm::vec3 Material::getKs() {
    return this->ks;
}

glm::vec3 Material::getKd() {
    return this->kd;
}

float Material::getNs() {
    return this->ns;
}

GLuint Material::getTID() {
    return this->TID;
}

void Material::setId(std::string id) {
    this->id = id;
}

void Material::setMapKd(std::string mapKd) {
    this->mapKd = mapKd;
}

void Material::setKa(glm::vec3 ka) {
    this->ka = ka;
}

void Material::setKs(glm::vec3 ks) {
    this->ks = ks;
}

void Material::setKd(glm::vec3 kd) {
    this->kd = kd;
}

void Material::setNs(float ns) {
    this->ns = ns;
}

void Material::setTID(GLuint TID) {
    this->TID = TID;
}
