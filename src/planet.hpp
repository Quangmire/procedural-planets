#ifndef PLANET_HPP
#define PLANET_HPP

#include <vector>

#define GL_GLEXT_PROTOTYPES 1
#define GL3_PROTOTYPES 1
#include <glm/glm.hpp>

#include "icosphere.hpp"
#include "render.hpp"
#include "config.hpp"

class Planet: public Renderable {
public:
    Planet();
    void render(int width, int height, float delta);
    void generate(const Configuration cfg);
private:
    Icosphere planet, clouds;
    VertexArrayObject vao_planet, vao_clouds, vao_lines;
    std::vector<Vertex> lines;
    std::vector<glm::uvec2> lines_index;
    Configuration config;
    glm::mat4 m, v, p, mvp;
    bool dirty = true;
};

#endif
