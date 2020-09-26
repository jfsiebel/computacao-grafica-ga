#ifndef COMPUTACAO_GRAFICA_GA_OBJREADER_H
#define COMPUTACAO_GRAFICA_GA_OBJREADER_H

#include <string>
#include <vector>

#include "Material.h"
#include "Mesh.h"

class OBJReader {
public:
    OBJReader();
    ~OBJReader();

    Mesh *read(std::string filename);
    std::vector<Material *> readMaterials(std::string filePath);

};

#endif //COMPUTACAO_GRAFICA_GA_OBJREADER_H
