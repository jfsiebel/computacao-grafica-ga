#ifndef COMPUTACAO_GRAFICA_GA_MATERIAL_H
#define COMPUTACAO_GRAFICA_GA_MATERIAL_H

#include <string>

#include <GL/glew.h>
#include "glm/vec3.hpp"

class Material {
private:
    std::string id, mapKd;
    glm::vec3 ka, kd, ks;
    float ns;
    GLuint TID;
public:
    Material();
    ~Material();

    std::string getId();
    std::string getMapKd();
    glm::vec3 getKa();
    glm::vec3 getKd();
    glm::vec3 getKs();
    float getNs();
    GLuint getTID();

    void setId(std::string id);
    void setMapKd(std::string mapKd);
    void setKa(glm::vec3 ka);
    void setKd(glm::vec3 kd);
    void setKs(glm::vec3 ks);
    void setNs(float ns);
    void setTID(GLuint TID);

};


#endif //COMPUTACAO_GRAFICA_GA_MATERIAL_H
