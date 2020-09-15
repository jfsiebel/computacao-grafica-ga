#ifndef COMPUTACAO_GRAFICA_GA_OBJREADER_H
#define COMPUTACAO_GRAFICA_GA_OBJREADER_H

#include <string>

#include "Mesh.h"

class OBJReader {
public:
    OBJReader();
    ~OBJReader();

    Mesh *read(std::string filename);

};

#endif //COMPUTACAO_GRAFICA_GA_OBJREADER_H
