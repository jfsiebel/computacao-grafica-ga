#include <iostream>
#include <ctgmath>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <vector>

#include "glm/vec2.hpp"
#include "glm/vec3.hpp"

#include "Face.h"
#include "Group.h"
#include "Mesh.h"
#include "Obj3D.h"

using namespace std;
using namespace glm;

int main() {
    if (!glfwInit()) {
        fprintf(stderr, "ERROR: could not start GLFW3\n");
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(1980, 1024, "computacao-grafica-ga", NULL, NULL);

    if (!window) {
        fprintf(stderr, "ERROR: could not open window with GLFW3\n");
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    glewInit();

    const GLubyte *renderer = glGetString(GL_RENDERER);
    const GLubyte *version = glGetString(GL_VERSION);

    printf("Renderer: %s\n", renderer);
    printf("OpenGL version: %s\n", version);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_ALWAYS);

    Mesh *mesh = new Mesh();

    mesh->addVertex(new glm::vec3(-0.5f, -0.5f, 0.0f));  //A 0
    mesh->addVertex(new glm::vec3(0.5f, -0.5f, 0.0f));   //B 1
    mesh->addVertex(new glm::vec3(-0.5f, 0.5f, 0.0f));   //C 2
    mesh->addVertex(new glm::vec3(0.5f, 0.5f, 0.0f));    //D 3
    mesh->addVertex(new glm::vec3(-0.5f, 0.5f, 1.0f));  //E 4
    mesh->addVertex(new glm::vec3(0.5f, 0.5f, 1.0f));   //F 5
    mesh->addVertex(new glm::vec3(-0.5f, -0.5f, 1.0f)); //G 6
    mesh->addVertex(new glm::vec3(0.5f, -0.5f, 1.0f));   //H 7

    Face *f1 = new Face();
    f1->addVert(0);
    f1->addVert(1);
    f1->addVert(2);
    f1->addVert(2);
    f1->addVert(3);
    f1->addVert(1);

    Face *f2 = new Face();
    f2->addVert(6);
    f2->addVert(7);
    f2->addVert(4);
    f2->addVert(4);
    f2->addVert(5);
    f2->addVert(7);

    Face *f3 = new Face();
    f3->addVert(0);
    f3->addVert(6);
    f3->addVert(2);
    f3->addVert(2);
    f3->addVert(4);
    f3->addVert(0);

    Face * f4 = new Face();
    f4->addVert(0);
    f4->addVert(1);
    f4->addVert(6);
    f4->addVert(6);
    f4->addVert(7);
    f4->addVert(1);

    Face * f5 = new Face();
    f5->addVert(2);
    f5->addVert(3);
    f5->addVert(4);
    f5->addVert(4);
    f5->addVert(5);
    f5->addVert(3);

    Face * f6 = new Face();
    f6->addVert(1);
    f6->addVert(7);
    f6->addVert(3);
    f6->addVert(3);
    f6->addVert(5);
    f6->addVert(7);

    Group *group = new Group();

    group->addFace(f1);
    group->addFace(f2);
    group->addFace(f3);
    group->addFace(f4);
    group->addFace(f5);
    group->addFace(f6);

    mesh->addGroup(group);

    for (Group *g : mesh->getGroups()) {
        vector<float> vs;
        for (Face *f : g->getFaces()) {
            for (int idx : f->getVerts()) {
                glm::vec3 *vertex = mesh->getVertices()[idx];

                vs.push_back(vertex->x);
                vs.push_back(vertex->y);
                vs.push_back(vertex->z);
            }
        }
        GLuint vbo, vao;
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vs.size() * sizeof(float), vs.data(), GL_STATIC_DRAW);

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

        g->setVAO(vao);
    }

    glClearColor(0.2f, 0.3f, 0.4f, 1.0f);

    Obj3D *obj = new Obj3D();

    obj->setMesh(mesh);

    const char *vertex_shader =
            "#version 410\n"
            "layout(location=0) in vec3 vp;"
            "layout(location=1) in vec3 vc;"
            "out vec3 color;"
            "void main() {"
            "   color = vc;"
            "   gl_Position = vec4(vp, 1.0);"
            "}";

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertex_shader, NULL);
    glCompileShader(vs);

    const char *fragment_shader =
            "#version 410\n"
            "in vec3 color;"
            "out vec4 frag_color;"
            "void main() {"
            "   frag_color = vec4(color, 1.0);"
            "}";

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragment_shader, NULL);
    glCompileShader(fs);

    GLuint shader_programme = glCreateProgram();
    glAttachShader(shader_programme, fs);
    glAttachShader(shader_programme, vs);
    glBindAttribLocation(shader_programme, 0, "vp");
    glBindAttribLocation(shader_programme, 1, "vc");
    glLinkProgram(shader_programme);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shader_programme);

        Mesh *m = obj->getMesh();

        for (Group *g : m->getGroups()) {
            glBindVertexArray(g->getVAO());
            glDrawArrays(GL_TRIANGLES, 0, g->getNumOfVertices());
        }

        glfwPollEvents();

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}