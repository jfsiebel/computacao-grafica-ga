#include <iostream>
#include <ctgmath>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <vector>

#include "glm/ext.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/type_ptr.hpp>

#include "Face.h"
#include "Group.h"
#include "Mesh.h"
#include "Obj3D.h"

#define WIDTH 1980
#define HEIGHT 1024

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

    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "computacao-grafica-ga", NULL, NULL);

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
    glClear(GL_LESS);

    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 cameraDirection = glm::normalize(cameraTarget - cameraPos);
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
    glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);

    glm::mat4 view(1.0f);
    view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

    glm::mat4 proj = glm::mat4(1.0f);
    proj = glm::perspective(glm::radians(60.0f), (float) WIDTH / (float) HEIGHT, 0.1f, 100.0f);

    Mesh *mesh = new Mesh();

    mesh->addVertex(new glm::vec3(-0.5f, -0.5f, 0.5f));  //A 0
    mesh->addVertex(new glm::vec3(0.5f, -0.5f, 0.5f));   //B 1
    mesh->addVertex(new glm::vec3(-0.5f, 0.5f, 0.5f));   //C 2
    mesh->addVertex(new glm::vec3(0.5f, 0.5f, 0.5f));    //D 3
    mesh->addVertex(new glm::vec3(-0.5f, 0.5f, -0.5f));  //E 4
    mesh->addVertex(new glm::vec3(0.5f, 0.5f, -0.5f));   //F 5
    mesh->addVertex(new glm::vec3(-0.5f, -0.5f, -0.5f)); //G 6
    mesh->addVertex(new glm::vec3(0.5f, -0.5f, -0.5f));   //H 7

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

    Face *f4 = new Face();
    f4->addVert(0);
    f4->addVert(1);
    f4->addVert(6);
    f4->addVert(6);
    f4->addVert(7);
    f4->addVert(1);

    Face *f5 = new Face();
    f5->addVert(2);
    f5->addVert(3);
    f5->addVert(4);
    f5->addVert(4);
    f5->addVert(5);
    f5->addVert(3);

    Face *f6 = new Face();
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

    Obj3D *obj = new Obj3D();

    obj->setMesh(mesh);

    const char *vertex_shader =
            "#version 410\n"
            "layout(location=0) in vec3 vp;"
            "uniform mat4 proj, view;"
            "void main() {"
            "   gl_Position = proj * view * vec4(vp, 1.0);"
            "}";

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertex_shader, NULL);
    glCompileShader(vs);

    const char *fragment_shader =
            "#version 410\n"
            "out vec4 frag_color;"
            "void main() {"
            "   frag_color = vec4(1.0, 1.0, 1.0, 1.0);"
            "}";

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragment_shader, NULL);
    glCompileShader(fs);

    GLuint shader_programme = glCreateProgram();
    glAttachShader(shader_programme, fs);
    glAttachShader(shader_programme, vs);
    glLinkProgram(shader_programme);

    int projLocation = glGetUniformLocation(shader_programme, "proj");
    int viewLocation = glGetUniformLocation(shader_programme, "view");

    glClearColor(0.6f, 0.6f, 0.8f, 1.0f);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//        const float radius = 10.0f;
//        float camX = sin(glfwGetTime()) * radius;
//        float camY = sin(glfwGetTime()) * radius;
//        float camZ = cos(glfwGetTime()) * radius;
//
//        view = glm::lookAt(glm::vec3(camX, camY, camZ), cameraTarget, up);

        glUseProgram(shader_programme);

        glUniformMatrix4fv(projLocation, 1, GL_FALSE, glm::value_ptr(proj));
        glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));

        Mesh *m = obj->getMesh();

        for (Group *g : m->getGroups()) {
            glBindVertexArray(g->getVAO());
            glDrawArrays(GL_TRIANGLES, 0, g->getNumOfVertices());
        }

        glfwPollEvents();
        glfwSwapBuffers(window);
        if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE)) {
            glfwSetWindowShouldClose(window, 1);
        }
    }

    glfwTerminate();
    return 0;
}