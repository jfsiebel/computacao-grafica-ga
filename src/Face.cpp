#include "Face.h"

Face::Face() {}
Face::~Face() {}

std::vector<int> Face::getVerts() {
    return this->verts;
}

std::vector<int> Face::getNorms() {
    return this->norms;
}

std::vector<int> Face::getTexts() {
    return this->texts;
}

void Face::addVert(int v)
{
    this->verts.push_back(v);
}

void Face::addNorm(int n)
{
    this->norms.push_back(n);
}

void Face::addText(int t)
{
    this->texts.push_back(t);
}
