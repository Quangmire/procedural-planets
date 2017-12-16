#include <iostream>
#define GL_GLEXT_PROTOTYPES 1
#define GL3_PROTOTYPES 1
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "icosphere.hpp"
#include "perlin.hpp"
#include "render.hpp"
#include "debuggl.h"
#include "config.hpp"
#include "planet.hpp"

static void error_callback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    if (key == GLFW_KEY_1 && action == GLFW_RELEASE)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    if (key == GLFW_KEY_2 && action == GLFW_RELEASE)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

int main(void) {
    GLFWwindow* window;

    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    window = glfwCreateWindow(640, 480, "Procedural Planets", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(window, key_callback);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glewExperimental = GL_TRUE;
    glewInit();
    glGetError();

    float ratio;
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    Configuration cfg;
    cfg.subdivisions = 5;
    cfg.cloud_subdivisions = 5;

    Planet planet;
    planet.generate(cfg);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    float time = glfwGetTime();
    int pkeys[GLFW_KEY_LAST];
    int keys[GLFW_KEY_LAST];
    while (!glfwWindowShouldClose(window))
    {
        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float) height;
        glViewport(0, 0, width, height);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDepthFunc(GL_LEQUAL);
        
        for(int i = GLFW_KEY_SPACE; i < GLFW_KEY_LAST; i++) {
            pkeys[i] = keys[i];
            keys[i] = glfwGetKey(window, i);
        }

        if(keys[GLFW_KEY_UP] == GLFW_RELEASE && pkeys[GLFW_KEY_UP] == GLFW_PRESS) {
            if(cfg.subdivisions < 8) {
                cfg.subdivisions = cfg.subdivisions + 1;
                cfg.cloud_subdivisions = cfg.cloud_subdivisions + 1;
                planet.generate(cfg);
            }
        }

        if(keys[GLFW_KEY_DOWN] == GLFW_RELEASE && pkeys[GLFW_KEY_DOWN] == GLFW_PRESS) {
            if(cfg.subdivisions > 0) {
                cfg.subdivisions = cfg.subdivisions - 1;
                cfg.cloud_subdivisions = cfg.cloud_subdivisions - 1;
                planet.generate(cfg);
            }
        }

        if(keys[GLFW_KEY_LEFT] == GLFW_RELEASE && pkeys[GLFW_KEY_LEFT] == GLFW_PRESS) {
            cfg.rotation_axis = glm::rotate(cfg.rotation_axis, glm::pi<float>()/32.0f, glm::vec3(0.0f, 0.0f, -1.0f));
            planet.generate(cfg);
        }

        if(keys[GLFW_KEY_RIGHT] == GLFW_RELEASE && pkeys[GLFW_KEY_RIGHT] == GLFW_PRESS) {
            cfg.rotation_axis = glm::rotate(cfg.rotation_axis, -glm::pi<float>()/32.0f, glm::vec3(0.0f, 0.0f, -1.0f));
            planet.generate(cfg);
        }

        if(keys[GLFW_KEY_3] == GLFW_RELEASE && pkeys[GLFW_KEY_3] == GLFW_PRESS) {
            if(cfg.show != cfg.PLANET) {
                cfg.show = cfg.PLANET;
                planet.generate(cfg);
            }
        }

        if(keys[GLFW_KEY_4] == GLFW_RELEASE && pkeys[GLFW_KEY_4] == GLFW_PRESS) {
            if(cfg.show != cfg.HEAT) {
                cfg.show = cfg.HEAT;
                planet.generate(cfg);
            }
        }

        if(keys[GLFW_KEY_5] == GLFW_RELEASE && pkeys[GLFW_KEY_5] == GLFW_PRESS) {
            if(cfg.show != cfg.MOISTURE) {
                cfg.show = cfg.MOISTURE;
                planet.generate(cfg);
            }
        }

        if(keys[GLFW_KEY_R] == GLFW_RELEASE && pkeys[GLFW_KEY_R] == GLFW_PRESS) {
            cfg.seed = rand() % 2560000 / 10000.0f;
            planet.generate(cfg);
        }

        if(keys[GLFW_KEY_W] == GLFW_RELEASE && pkeys[GLFW_KEY_W] == GLFW_PRESS) {
            cfg.sea_level += 0.05f;
            if(cfg.sea_level > 1.0f) {
                cfg.sea_level = 1.0f;
            }
            planet.generate(cfg);
        }

        if(keys[GLFW_KEY_S] == GLFW_RELEASE && pkeys[GLFW_KEY_S] == GLFW_PRESS) {
            cfg.sea_level -= 0.05f;
            if(cfg.sea_level < 0.0f) {
                cfg.sea_level = 0.0f;
            }
            planet.generate(cfg);
        }

        if(keys[GLFW_KEY_P] == GLFW_RELEASE && pkeys[GLFW_KEY_P] == GLFW_PRESS) {
            cfg.rotating = !cfg.rotating;
            planet.generate(cfg);
        }

        if(keys[GLFW_KEY_Q] == GLFW_RELEASE && pkeys[GLFW_KEY_Q] == GLFW_PRESS) {
            cfg.camera_distance += 0.5f;
            if(cfg.camera_distance > cfg.radius * 10) {
                cfg.camera_distance = cfg.radius * 10;
            }
            planet.generate(cfg);
        }

        if(keys[GLFW_KEY_E] == GLFW_RELEASE && pkeys[GLFW_KEY_E] == GLFW_PRESS) {
            cfg.camera_distance -= 0.5f;
            if(cfg.camera_distance < cfg.radius + 0.5f) {
                cfg.camera_distance = cfg.radius + 0.5f;
            }
            planet.generate(cfg);
        }
        
        planet.render(width, height, glfwGetTime() - time);
        time = glfwGetTime();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
