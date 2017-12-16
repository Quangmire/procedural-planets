#include "render.hpp"

#include <GLFW/glfw3.h>

#include "debuggl.h"

VertexArrayObject::VertexArrayObject(std::vector<std::string> vbo_names) {
    // Generate and bind vertex array object
    CHECK_GL_ERROR(glGenVertexArrays(1, &vao));
    CHECK_GL_ERROR(glBindVertexArray(vao));

    // Generate vbos
    std::vector<GLuint> vbo_ids = {0, 0};
    CHECK_GL_ERROR(glGenBuffers(vbo_names.size(), vbo_ids.data()));
    for(unsigned i = 0; i < vbo_names.size(); i++) {
        vbos[vbo_names[i]] = vbo_ids[i];
    }
};

void VertexArrayObject::shaders(
        std::vector<const char*> shaders,   // Vertex, Geometry, Fragment
        std::vector<std::string> attribs,
        uint64_t vertex_size,
        std::vector<uint64_t> offsets,
        std::vector<std::string> uniform_names,
        std::vector<UniformBinder> binders
        ) {

    // Create shaders
    CHECK_GL_ERROR(vertex_shader = glCreateShader(GL_VERTEX_SHADER));
    CHECK_GL_ERROR(glShaderSource(vertex_shader, 1, &shaders[0], nullptr));
    glCompileShader(vertex_shader);
    CHECK_GL_SHADER_ERROR(vertex_shader);

    if(shaders[1]) {
        geometry_shader = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometry_shader, 1, &shaders[1], nullptr);
        glCompileShader(geometry_shader);
    }

    CHECK_GL_ERROR(fragment_shader = glCreateShader(GL_FRAGMENT_SHADER));
    CHECK_GL_ERROR(glShaderSource(fragment_shader, 1, &shaders[2], nullptr));
    glCompileShader(fragment_shader);
    CHECK_GL_SHADER_ERROR(fragment_shader);

    // Create Shader Program + Attach Shaders
    CHECK_GL_ERROR(shader_program = glCreateProgram());
    glAttachShader(shader_program, vertex_shader);
    if(shaders[1]) {
        glAttachShader(shader_program, geometry_shader);
    }
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);
    CHECK_GL_PROGRAM_ERROR(shader_program);

    // Create Attributes (Attributes are per vertex)
    for(unsigned i = 0; i < attribs.size(); i++) {
        auto name = attribs[i].c_str();
        GLuint loc;
        CHECK_GL_ERROR(loc = glGetAttribLocation(shader_program, name));
        CHECK_GL_ERROR(glEnableVertexAttribArray(loc));
        CHECK_GL_ERROR(glVertexAttribPointer(loc,
                // Size of this attrib
                i == attribs.size() - 1 ? vertex_size - offsets[i] : offsets[i + 1] - offsets[i],
                GL_FLOAT, GL_FALSE,
                sizeof(float) * vertex_size, // stride
                (void *) (sizeof(float) * offsets[i])));         // offset

    }

    // Create Uniforms (Uniforms are constant for all vertices)
    for(unsigned i = 0; i < uniform_names.size(); i++) {
        auto name = uniform_names[i].c_str();
        GLint loc;
        CHECK_GL_ERROR(loc = glGetUniformLocation(shader_program, name));
        uniforms[uniform_names[i]] = {
            binders[i],
            loc
        };
    }
}

void VertexArrayObject::update_data(std::string name, GLuint type, unsigned size, void* data) {
    CHECK_GL_ERROR(glBindVertexArray(vao));
    CHECK_GL_ERROR(glBindBuffer(type, vbos[name]));
    CHECK_GL_ERROR(glBufferData(type, size, data, GL_STATIC_DRAW));
}

void VertexArrayObject::setup() {
    CHECK_GL_ERROR(glBindVertexArray(vao));
    CHECK_GL_ERROR(glUseProgram(shader_program));
    for(auto iter = uniforms.begin(); iter != uniforms.end(); iter++) {
        auto uniform = *iter;
        iter->second.bind(iter->second.loc);
    }
}
