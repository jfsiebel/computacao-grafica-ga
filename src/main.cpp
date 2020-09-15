#define GLM_ENABLE_EXPERIMENTAL
#define WIDTH 1980
#define HEIGHT 1024

#include <iostream>
#include <ctgmath>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <math.h>
#include <vector>

#include "glm/ext.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Face.h"
#include "Group.h"
#include "Mesh.h"
#include "Obj3D.h"
#include "OBJReader.h"

using namespace std;
using namespace glm;

bool firstMouse = true;
float tyaw   = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float tpitch =  0.0f;
float lastX =  WIDTH / 2.0;
float lastY =  HEIGHT / 2.0;
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);

void mouse_callback(GLFWwindow* window, double xpos, double ypos);

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
    glfwSetCursorPosCallback(window, mouse_callback);

    glewExperimental = GL_TRUE;
    glewInit();

    const GLubyte *renderer = glGetString(GL_RENDERER);
    const GLubyte *version = glGetString(GL_VERSION);

    printf("Renderer: %s\n", renderer);
    printf("OpenGL version: %s\n", version);

    glEnable(GL_DEPTH_TEST);
    glClear(GL_DEPTH_BUFFER_BIT);

    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    glm::vec3 cameraDirection = glm::normalize(cameraTarget - cameraPos);
//    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
    glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);

    glm::mat4 view(1.0f);

    glm::mat4 proj(1.0f);
    proj = glm::perspective(glm::radians(60.0f), (float) WIDTH / (float) HEIGHT, 0.1f, 100.0f);

    OBJReader *objReader = new OBJReader();

    Mesh *mesa = objReader->read("../obj/mesa/mesa01.obj");

    for (Group *g : mesa->getGroups()) {
        vector<float> vs;
        vector<float> vts;
        vector<float> vns;
        for (Face *f : g->getFaces()) {
            for (int idx : f->getVerts()) {
                glm::vec3 *vertex = mesa->getVertices()[idx];

                vs.push_back(vertex->x);
                vs.push_back(vertex->y);
                vs.push_back(vertex->z);
            }
            for (int idx : f->getTexts()) {
                glm::vec2 *textures = mesa->getMappings()[idx];

                vts.push_back(textures->x);
                vts.push_back(textures->y);
            }
            for (int idx : f->getNorms()) {
                glm::vec3 *normals = mesa->getNormals()[idx];

                vns.push_back(normals->x);
                vns.push_back(normals->y);
                vns.push_back(normals->z);
            }
        }
        GLuint VAO, VBOVertices, VBOTextures, VBONormals;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBOVertices);
        glGenBuffers(1, &VBOTextures);
        glGenBuffers(1, &VBONormals);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBOVertices);
        glBufferData(GL_ARRAY_BUFFER, vs.size() * sizeof(float), vs.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, VBOTextures);
        glBufferData(GL_ARRAY_BUFFER, vts.size() * sizeof(float), vts.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, VBONormals);
        glBufferData(GL_ARRAY_BUFFER, vns.size() * sizeof(float), vns.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(2);

        g->setVAO(VAO);
    }

    Obj3D *obj = new Obj3D();

    obj->setMesh(mesa);

    const char *vertex_shader =
            "#version 410\n"
            "layout(location=0) in vec3 vp;"
            "uniform mat4 proj, scale, view;"
            "void main() {"
            "   gl_Position = proj * view * scale * vec4(vp, 1.0);"
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

    glm::mat4 scale(1.0f);

    int projLocation = glGetUniformLocation(shader_programme, "proj");
    int viewLocation = glGetUniformLocation(shader_programme, "view");
    int scaleLocation = glGetUniformLocation(shader_programme, "scale");

    glClearColor(0.6f, 0.6f, 0.8f, 1.0f);

    double cameraSpeed = 2.5f;
    double deltaTime = 0.0f;
    double lastFrame = 0.0f;

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        double currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE)) {
            glfwSetWindowShouldClose(window, 1);
        }
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
//            cout << glm::to_string(cameraPos) << endl;
            cameraPos += cameraSpeed * deltaTime * cameraFront;
//            cout << glm::to_string(cameraPos) << endl;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            cameraPos -= cameraSpeed * deltaTime  * cameraFront;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed * deltaTime;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed * deltaTime;
        }

        Mesh *m = obj->getMesh();

        float d = glm::distance(m->getMin(), m->getMax());

        glUseProgram(shader_programme);

        glUniformMatrix4fv(projLocation, 1, GL_FALSE, glm::value_ptr(proj));

        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));

        scale = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f / d, 1.0f / d, 1.0f / d));
        glUniformMatrix4fv(scaleLocation, 1, GL_FALSE, glm::value_ptr(scale));

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

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f; // change this value to your liking
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    tyaw += xoffset;
    tpitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (tpitch > 89.0f)
        tpitch = 89.0f;
    if (tpitch < -89.0f)
        tpitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(tyaw)) * cos(glm::radians(tpitch));
    front.y = sin(glm::radians(tpitch));
    front.z = sin(glm::radians(tyaw)) * cos(glm::radians(tpitch));
    cameraFront = glm::normalize(front);
}