#ifndef RENDER_HPP
#define RENDER_HPP

#define GL_GLEXT_PROTOTYPES 1
#define GL3_PROTOTYPES 1
#include <GLFW/glfw3.h>

#include <unordered_map>
#include <vector>
#include <functional>
#include <string>

#include "debuggl.h"

typedef std::function<void(GLint)> UniformBinder;

struct Uniform {
    UniformBinder bind;
    GLint loc;
};

class VertexArrayObject {
private:
    GLuint vao;
    std::unordered_map<std::string, GLuint> vbos;
    GLuint vertex_shader, geometry_shader, fragment_shader, shader_program;
    std::unordered_map<std::string, Uniform> uniforms;
public:
    VertexArrayObject(std::vector<std::string> vbo_names);
    void shaders(
            std::vector<const char*> shaders,   // Vertex, Geometry, Fragment
            std::vector<std::string> attribs, uint64_t vertex_size,
            std::vector<uint64_t> offsets,
            std::vector<std::string> uniform_names,
            std::vector<UniformBinder> binders);
    void update_data(std::string name, GLuint type, unsigned size, void* data);
    void setup();
};

class Renderable {
public:
    virtual void render(int width, int height, float delta) = 0;
};

#endif
