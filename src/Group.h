#ifndef COMPUTACAO_GRAFICA_GA_GROUP_H
#define COMPUTACAO_GRAFICA_GA_GROUP_H

#include <string>
#include <vector>

#include "GL/glew.h"

#include "Face.h"

class Group
{
private:
    std::vector<Face *> faces;
    std::string material;
    std::string name;
    GLuint VAO;
    int numOfVertices;

public:
    Group();
    ~Group();

    std::vector<Face *> getFaces();
    std::string getMaterial();
    std::string getName();
    GLuint getVAO();
    int getNumOfVertices();

    void addFace(Face *face);

    void setFaces(std::vector<Face *> faces);
    void setMaterial(std::string material);
    void setName(std::string name);
    void setVAO(GLuint VAO);
};

#endif //COMPUTACAO_GRAFICA_GA_GROUP_H