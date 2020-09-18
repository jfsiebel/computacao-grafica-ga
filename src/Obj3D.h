#ifndef COMPUTACAO_GRAFICA_GA_OBJ3D_H
#define COMPUTACAO_GRAFICA_GA_OBJ3D_H

#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"

#include "Mesh.h"

class Obj3D
{
private:
    bool eliminavel;
    glm::vec3 direcao;
    bool isVisible;
    Mesh *mesh;
    int status;
    glm::mat4 transform;

public:
    Obj3D();
    ~Obj3D();

    bool getEliminavel();
    glm::vec3 getDirecao();
    bool getIsVisible();
    Mesh *getMesh();
    int getStatus();
    glm::mat4 getTransform();

    void setEliminavel(bool eliminavel);
    void setDirecao(glm::vec3 direcao);
    void setIsVisible(bool isVisible);
    void setMesh(Mesh *mesh);
    void setStatus(int status);
    void setTransform(glm::mat4 transform);
};

#endif //COMPUTACAO_GRAFICA_GA_OBJ3D_H