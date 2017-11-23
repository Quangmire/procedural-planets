#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "icosphere.h"
#include "perlin.h"
#include "render.h"
#include "debuggl.h"

static const char* vertex_shader_text =
"uniform mat4 MVP;\n"
"attribute vec3 vPos;\n"
"attribute vec4 vCol;\n"
"varying vec4 color;\n"
"void main()\n"
"{\n"
"    gl_Position = MVP * vec4(vPos, 1.0);\n"
"    color = vCol;\n"
"}\n";

static const char* fragment_shader_text =
"varying vec4 color;\n"
"void main()\n"
"{\n"
"    gl_FragColor = color;\n"
"}\n";

static void error_callback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
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

    window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
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

    // Generate the planet
    float radius = 4.0f;
    float camera_distance = 10.0f;
    Icosphere planet;
    for(int i = 0; i < 8; i++) {
        planet.subdivide();
    }
    for(int i = 0; i < planet.vertices.size(); i++) {
        auto vert = planet.vertices[i];
        auto v = vert.vertex;
        // Add 2 to move away from origin. Perlin does not handle negatives well
        float noise = octave_perlin(v.x/4.0f + 2, v.y/4.0f + 2, v.z/4.0f + 2, 4, 3);
        noise = noise * 0.1f - 0.05f;
        planet.vertices[i] = {noise >= 0.0f ? (noise + radius) * v : v * radius,
            noise >= 0.0f ?
                glm::vec4(0.0f, 0.4f + 2 * noise, 0.0f, 1.0f) :
                glm::vec4(0.0f, 0.0f, 0.4f + 2 * noise, 1.0f)};
    }

    // Generate clouds
    float cloud_radius = radius + 0.1f;
    Icosphere clouds;
    for(int i = 0; i < 7; i++) {
        clouds.subdivide();
    }
    for(int i = 0; i < clouds.vertices.size(); i++) {
        auto vert = clouds.vertices[i];
        auto v = vert.vertex;
        float noise = octave_perlin(v.x/3.f + 2, v.y/3.f + 2, v.z/3.f + 2, 4, 2);
        noise = noise * 0.6f - 0.3f;
        clouds.vertices[i] = {noise >= 0.0f ? (noise + cloud_radius) * v : v * cloud_radius,
            noise >= 0.0f ?
                glm::vec4(1.0f, 1.0f, 1.0f, 10 * noise) :
                glm::vec4(1.0f, 1.0f, 1.0f, 0.0f)};
    }

    float ratio;
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    ratio = width / (float) height;
    glm::mat4 m = glm::rotate(glm::mat4(1.0f), float(glfwGetTime()), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 v = glm::lookAt(glm::vec3(0.0f, 0.0f, -camera_distance), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 p = glm::perspective(45.0f, ratio, 0.0001f, 1000.0f);
    glm::mat4 mvp = p * v * m;

    VertexArrayObject planet_render({"vertex", "index"});
    planet_render.update_data("vertex", GL_ARRAY_BUFFER, planet.vertices.size() * sizeof(float) * 7, planet.vertices.data());
    planet_render.update_data("index", GL_ELEMENT_ARRAY_BUFFER, planet.triangles.size() * sizeof(uint32_t) * 3, planet.triangles.data());
    planet_render.shaders(
            {vertex_shader_text, nullptr, fragment_shader_text},
            {"vPos", "vCol"}, 7, {0, 3}, {"MVP"},
            {[&mvp](GLint loc){glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mvp));}}
            );

    VertexArrayObject clouds_render({"vertex", "index"});
    clouds_render.update_data("vertex", GL_ARRAY_BUFFER, clouds.vertices.size() * sizeof(float) * 7, clouds.vertices.data());
    clouds_render.update_data("index", GL_ELEMENT_ARRAY_BUFFER, clouds.triangles.size() * sizeof(uint32_t) * 3, clouds.triangles.data());
    clouds_render.shaders(
            {vertex_shader_text, nullptr, fragment_shader_text},
            {"vPos", "vCol"}, 7, {0, 3}, {"MVP"},
            {[&mvp](GLint loc){glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mvp));}}
            );

    while (!glfwWindowShouldClose(window))
    {
        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float) height;
        glViewport(0, 0, width, height);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDepthFunc(GL_LESS);

        p = glm::perspective(45.0f, ratio, 0.0001f, 1000.0f);
        m = glm::rotate(glm::mat4(1.0f), float(glfwGetTime()/4), glm::vec3(0.0f, 1.0f, 0.0f));
        mvp = p * v * m;

        planet_render.setup();
        CHECK_GL_ERROR(glDrawElements(GL_TRIANGLES, planet.triangles.size() * 3, GL_UNSIGNED_INT, 0));
        clouds_render.setup();
        CHECK_GL_ERROR(glDrawElements(GL_TRIANGLES, clouds.triangles.size() * 3, GL_UNSIGNED_INT, 0));
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
