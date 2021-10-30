#define GLM_ENABLE_EXPERIMENTAL
#define STB_IMAGE_IMPLEMENTATION

#define WIDTH 1980
#define HEIGHT 1024

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <ctgmath>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <vector>
#include <map>

#include "glm/ext.hpp"

#include "Face.h"
#include "Group.h"
#include "Mesh.h"
#include "Obj3D.h"
#include "OBJReader.h"
#include "stb_image.h"

using namespace std;

struct Scene {
    map<string, Mesh *> malhas;
    map<string, Material *> materials;
    vector<Obj3D *> objects;
    Obj3D *bulletObj;
};

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
    glEnable(GL_TEXTURE_2D);
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Scene scene;

    string BASE_OBJ_PATH = "../obj/";

    OBJReader *objReader = new OBJReader();
    GLuint VAO, VBOVertices, VBOTextures, VBONormals, VBOMaterial;

    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    int qtdMesh = 0;
    int qtdObj = 0;

    ifstream arq("../src/config.cfg");

    while (!arq.eof()) {
        stringstream ss;
        string cfgLine;
        getline(arq, cfgLine);
        ss << cfgLine;
        string instruction;
        ss >> instruction;

        if (instruction == "#" || instruction.empty()) {
            continue;
        } else if (instruction == "camera") {
            float px = 0.0f, py = 0.0f, pz = 0.0f;
            float tx = 0.0f, ty = 0.0f, tz = 0.0f;
            float ux = 0.0f, uy = 0.0f, uz = 0.0f;

            getline(arq, cfgLine);
            ss << cfgLine;

            ss >> px >> py >> pz;
            ss >> tx >> ty >> tz;
            ss >> ux >> uy >> uz;

            cameraPos = glm::vec3(px, py, pz);
            cameraTarget = glm::vec3(tx, ty, tz);
            up = glm::vec3(ux, uy, uz);
        } else {
            if (qtdMesh == 0) {
                qtdMesh = stoi(instruction);

                for (int i = 0; i < qtdMesh; ++i) {
                    getline(arq, cfgLine);
                    ss.str("");
                    ss.clear();
                    ss << cfgLine;

                    string label, path;
                    ss >> label >> path;

                    Mesh *m = objReader->read(BASE_OBJ_PATH + path);

                    /**
                     * Ler mtl e gerar os materials
                     * */

                    if (!m->getMtllib().empty()) {
                        vector<Material *> materials = objReader->readMaterials(BASE_OBJ_PATH + m->getMtllib());

                        for (Material *material : materials) {
                            glGenTextures(1, &VBOMaterial);
                            glBindTexture(GL_TEXTURE_2D, VBOMaterial);

                            int imgWidth, imgHeight, nrChannels;
                            string imgPath =  BASE_OBJ_PATH + material->getMapKd();
                            unsigned char *data = stbi_load(imgPath.c_str(), &imgWidth, &imgHeight, &nrChannels, 0);

                            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                            glGenerateMipmap(GL_TEXTURE_2D);
                            stbi_image_free(data);

                            material->setTID(VBOMaterial);

                            scene.materials.insert({ material->getId(), material });
                        }
                    }

                    scene.malhas.insert({label, m});

                    for (Group *g : m->getGroups()) {
                        vector<float> vs;
                        vector<float> vts;
                        vector<float> vns;
                        for (Face *f : g->getFaces()) {
                            for (int idx : f->getVerts()) {
                                glm::vec3 *vertex = m->getVertices().at(idx);

                                vs.push_back(vertex->x);
                                vs.push_back(vertex->y);
                                vs.push_back(vertex->z);
                            }
                            for (int idx : f->getTexts()) {
                                glm::vec2 *textures = m->getMappings().at(idx);

                                vts.push_back(textures->x);
                                vts.push_back(textures->y);
                            }
                            for (int idx : f->getNorms()) {
                                glm::vec3 *normals = m->getNormals().at(idx);

                                vns.push_back(normals->x);
                                vns.push_back(normals->y);
                                vns.push_back(normals->z);
                            }
                        }
                        glGenVertexArrays(1, &VAO);
                        glGenBuffers(1, &VBOVertices);
                        glGenBuffers(1, &VBOTextures);
                        glGenBuffers(1, &VBONormals);

                        glBindVertexArray(VAO);

                        glBindBuffer(GL_ARRAY_BUFFER, VBOVertices);
                        glBufferData(GL_ARRAY_BUFFER, vs.size() * sizeof(GL_FLOAT), vs.data(), GL_STATIC_DRAW);
                        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
                        glEnableVertexAttribArray(0);

                        glBindBuffer(GL_ARRAY_BUFFER, VBOTextures);
                        glBufferData(GL_ARRAY_BUFFER, vts.size() * sizeof(GL_FLOAT), vts.data(), GL_STATIC_DRAW);
                        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
                        glEnableVertexAttribArray(1);

                        glBindBuffer(GL_ARRAY_BUFFER, VBONormals);
                        glBufferData(GL_ARRAY_BUFFER, vns.size() * sizeof(GL_FLOAT), vns.data(), GL_STATIC_DRAW);
                        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, NULL);
                        glEnableVertexAttribArray(2);

                        g->setVAO(VAO);
                    }
                }
            } else {
                qtdObj = stoi(instruction);

                for (int i = 0; i < qtdObj; i++) {
                    getline(arq, cfgLine);
                    ss.str("");
                    ss.clear();
                    ss << cfgLine;

                    string id = "";
                    string mesh = "";
                    float tx = 0.0f, ty = 0.0f, tz = 0.0f, scale = 0.0f, pr = 0.0f;
                    int isEliminavel = 0, isBullet = 0;

                    ss >> id >> mesh >> tx >> ty >> tz >> scale >> pr >> isEliminavel >> isBullet;

                    Obj3D *obj = new Obj3D();

                    obj->setMesh(scene.malhas.find(mesh)->second);
                    obj->setIsVisible(true);
                    obj->setEliminavel(isEliminavel);
                    scene.objects.push_back(obj);

                    if (isBullet) {
                        obj->setEliminavel(true);
                        obj->setIsVisible(false);
                        scene.bulletObj = obj;
                    }

                    obj->setTransform(glm::translate(glm::mat4(1.0f), glm::vec3(tx, ty, tz)));
                }
            }
        }
    }
    arq.close();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    const char *vertex_shader =
            "#version 410\n"
            "layout(location=0) in vec3 vp;"
            "layout(location=1) in vec2 vt;"
            "layout(location=2) in vec3 vn;"
            "uniform mat4 proj, model, view;"
            "out vec2 texCoord;"
            "void main() {"
            "   texCoord = vt;"
            "   gl_Position = proj * view * model * vec4(vp, 1.0);"
            "}";

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertex_shader, NULL);
    glCompileShader(vs);

    const char *fragment_shader =
            "#version 410\n"
            "in vec2 texCoord;"
            "out vec4 frag_color;"
            "uniform sampler2D tex;"
            "void main() {"
            "   frag_color = texture(tex, texCoord);"
            "}";

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragment_shader, NULL);
    glCompileShader(fs);

    GLuint shader_programme = glCreateProgram();
    glAttachShader(shader_programme, fs);
    glAttachShader(shader_programme, vs);
    glLinkProgram(shader_programme);

    glm::mat4 proj(1.0f);
    glm::mat4 view(1.0f);
    glm::mat4 model(1.0f);

    proj = glm::perspective(glm::radians(60.0f), (float) WIDTH / (float) HEIGHT, 0.1f, 100.0f);

    glm::vec3 cameraDirection = glm::normalize(cameraTarget - cameraPos);
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
    glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);

    int projLocation = glGetUniformLocation(shader_programme, "proj");
    int viewLocation = glGetUniformLocation(shader_programme, "view");
    int modelLocation = glGetUniformLocation(shader_programme, "model");

    glClearColor(0.6f, 0.6f, 0.8f, 1.0f);

    double cameraSpeed = 2.5f;
    double bulletSpeed = 5.0f;
    double deltaTime = 0.0f;
    double lastFrame = 0.0f;
    float yaw = -90.0f;
    float pitch = 0.0f;
    glm::vec3 bulletPosition;

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        double currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE)) {
            glfwSetWindowShouldClose(window, 1);
        }
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
            cameraPos += cameraSpeed * deltaTime * cameraFront;
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            cameraPos -= cameraSpeed * deltaTime * cameraFront;
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            glm::vec3 front(1.0f);
            yaw -= 1.0f;
            front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
            front.y = sin(glm::radians(pitch));
            front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
            cameraFront = glm::normalize(front);
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            glm::vec3 front(1.0f);
            yaw += 1.0f;
            front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
            front.y = sin(glm::radians(pitch));
            front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
            cameraFront = glm::normalize(front);
        }

        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            Obj3D *bullet = scene.bulletObj;
            if (!bullet->getIsVisible()) {
                bullet->setIsVisible(true);
                glm::vec3 min, max;
                min = bullet->getMesh()->getMin();
                max = bullet->getMesh()->getMax();
                glm::vec3 medio = (min + max) / 2;
                glm::mat4 t = glm::translate(glm::mat4(1.0f), cameraPos + cameraFront - medio);
                bullet->setTransform(t);
                bullet->setDirecao(cameraFront);
                bulletPosition = cameraPos + cameraFront;
            }
        }

        glUseProgram(shader_programme);

        glUniformMatrix4fv(projLocation, 1, GL_FALSE, glm::value_ptr(proj));

        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));

        for (Obj3D *obj : scene.objects) {
            if (obj->getIsVisible()) {
                Mesh *m = obj->getMesh();
                float d = glm::distance(m->getMin(), m->getMax());

                glm::mat4 translate = obj->getTransform();

                if (obj == scene.bulletObj) {
                    translate = glm::translate(translate, obj->getDirecao() * deltaTime * bulletSpeed);

                    obj->setTransform(translate);
                    bulletPosition += obj->getDirecao() * deltaTime * bulletSpeed;

                    if (distance(obj->getDirecao(), bulletPosition) > 15.0f) {
                        scene.bulletObj->setIsVisible(false);
                    }
                } else {
                    Obj3D *bullet = scene.bulletObj;

                    if (bullet->getIsVisible()) {
                        glm::vec3 bulletMin = bullet->getMesh()->getMin();
                        glm::vec3 bulletMax = bullet->getMesh()->getMax();

                        float bulletDistance = glm::distance(bulletMin, bulletMax);
                        glm::mat4 bulletModel = glm::scale(bullet->getTransform(),
                        glm::vec3(1.0f / bulletDistance, 1.0f / bulletDistance,
                        1.0f / bulletDistance));
                        glm::vec3 bulletP0 = glm::vec3(bulletModel * glm::vec4(bulletMin, 1.0f));
                        glm::vec3 bulletP1 = glm::vec3(bulletModel * glm::vec4(bulletMax, 1.0f));
                        glm::vec3 bulletCenter = (bulletP0 + bulletP1) / 2;
                        float bulletRadius = glm::distance(bulletP0, bulletP1) / 2;

                        glm::mat4 objModel = glm::scale(obj->getTransform(), glm::vec3(1.0f / d, 1.0f / d, 1.0f / d));
                        glm::vec3 objP0 = glm::vec3(objModel * glm::vec4(m->getMin(), 1.0f));
                        glm::vec3 objP1 = glm::vec3(objModel * glm::vec4(m->getMax(), 1.0f));
                        glm::vec3 objCenter = (objP0 + objP1) / 2;
                        float objRadius = glm::distance(objP0, objP1) / 2;
                        glm::vec3 objVp = objCenter - bulletCenter;
                        glm::vec3 objP = glm::normalize(objVp) * bulletRadius + bulletCenter;

                        float collisionDistance = glm::distance(objCenter, bulletCenter);

                        if (collisionDistance < (bulletRadius + objRadius)) {
                            if ( obj->getEliminavel() ) {
                                obj->setIsVisible(false);
                                scene.bulletObj->setIsVisible(false);
                                continue;
                            } else {
                                glm::vec3 collisionNormal = (objP - objCenter)/objRadius;

                                float dot = glm::dot(collisionNormal, bulletPosition);

                                float rX = 2 * collisionNormal.x * dot - bulletPosition.x;
                                float rZ = 2 * collisionNormal.z * dot - bulletPosition.z;

                                scene.bulletObj->setDirecao(glm::vec3(rX, bulletPosition.y, rZ));
                            }
                        }
                    }
                }

                model = glm::scale(translate, glm::vec3(1.0f / d, 1.0f / d, 1.0f / d));
                glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
                for (Group *g : m->getGroups()) {
                    glBindVertexArray(g->getVAO());
                    if (!g->getMaterial().empty()) {
                        Material * groupMaterial = scene.materials.find(g->getMaterial())->second;
                        glBindTexture(GL_TEXTURE_2D, groupMaterial->getTID());
                    }
                    glDrawArrays(GL_TRIANGLES, 0, g->getNumOfVertices());
                }
            }
        }

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}