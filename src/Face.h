#ifndef COMPUTACAO_GRAFICA_GA_FACE_H
#define COMPUTACAO_GRAFICA_GA_FACE_H

#include <vector>

class Face
{
private:
    std::vector<int> verts;
    std::vector<int> norms;
    std::vector<int> texts;

public:
    Face();
    ~Face();

    std::vector<int> getVerts();
    std::vector<int> getNorms();
    std::vector<int> getTexts();



    void addVert(int v);
    void addNorm(int n);
    void addText(int t);
};

#endif //COMPUTACAO_GRAFICA_GA_FACE_H