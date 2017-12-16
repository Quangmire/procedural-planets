#include "planet.hpp"
#include "perlin.hpp"

#include <GLFW/glfw3.h>

#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "debuggl.h"

const char* vertex_shader_text =
#include "shaders/passthrough.vert"
;

const char* fragment_shader_text =
#include "shaders/passthrough.frag"
;

// Borrowed Colors + Discretization of Biomes below from jgallant
glm::vec4 Ice = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
glm::vec4 Desert = glm::vec4(238/255.0f, 218/255.0f, 130/255.0f, 1);
glm::vec4 Savanna = glm::vec4(177/255.0f, 209/255.0f, 110/255.0f, 1);
glm::vec4 TropicalRainforest = glm::vec4(66/255.0f, 123/255.0f, 25/255.0f, 1);
glm::vec4 Tundra = glm::vec4(96/255.0f, 131/255.0f, 112/255.0f, 1);
glm::vec4 TemperateRainforest = glm::vec4(29/255.0f, 73/255.0f, 40/255.0f, 1);
glm::vec4 Grassland = glm::vec4(164/255.0f, 225/255.0f, 99/255.0f, 1);
glm::vec4 SeasonalForest = glm::vec4(73/255.0f, 100/255.0f, 35/255.0f, 1);
glm::vec4 BorealForest = glm::vec4(95/255.0f, 115/255.0f, 62/255.0f, 1);
glm::vec4 Woodland = glm::vec4(139/255.0f, 175/255.0f, 90/255.0f, 1);

glm::vec4 biomes[6][6] = {
    //COLDEST //COLDER //COLD        //HOT                //HOTTER             //HOTTEST
    { Ice,    Tundra,  Grassland,    Desert,              Desert,              Desert },              //DRYEST
    { Ice,    Tundra,  Grassland,    Desert,              Desert,              Desert },              //DRYER
    { Ice,    Tundra,  Woodland,     Woodland,            Savanna,             Savanna },             //DRY
    { Ice,    Tundra,  BorealForest, Woodland,            Savanna,             Savanna },             //WET
    { Ice,    Tundra,  BorealForest, SeasonalForest,      TropicalRainforest,  TropicalRainforest },  //WETTER
    { Ice,    Tundra,  BorealForest, TemperateRainforest, TropicalRainforest,  TropicalRainforest }   //WETTEST
};

Planet::Planet():
    vao_planet({"vertex", "index"}),
    vao_clouds({"vertex", "index"}),
    vao_lines({"vertex", "index"})
{
    m = glm::mat4(1.0f);
    v = glm::lookAt(glm::vec3(0.0f, 0.0f, -config.camera_distance),
                    glm::vec3(0.0f, 0.0f, 0.0f),
                    glm::vec3(0.0f, 1.0f, 0.0f));
    mvp = glm::mat4(1.0f);

    // Planet
    vao_planet.update_data("vertex", GL_ARRAY_BUFFER, planet.vertices.size() * sizeof(float) * 7, planet.vertices.data());
    vao_planet.update_data("index", GL_ELEMENT_ARRAY_BUFFER, planet.triangles.size() * sizeof(uint32_t) * 3, planet.triangles.data());
    vao_planet.shaders(
            {vertex_shader_text, nullptr, fragment_shader_text},
            {"vPos", "vCol"}, 7, {0, 3}, {"MVP"},
            {[this](GLint loc){glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(this->mvp));}}
            );

    // Clouds
    vao_clouds.update_data("vertex", GL_ARRAY_BUFFER, clouds.vertices.size() * sizeof(float) * 7, clouds.vertices.data());
    vao_clouds.update_data("index", GL_ELEMENT_ARRAY_BUFFER, clouds.triangles.size() * sizeof(uint32_t) * 3, clouds.triangles.data());
    vao_clouds.shaders(
            {vertex_shader_text, nullptr, fragment_shader_text},
            {"vPos", "vCol"}, 7, {0, 3}, {"MVP"},
            {[this](GLint loc){glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(this->mvp));}}
            );

    // Lines
    lines.push_back({glm::vec3(0.0f, -3.0f, 0.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f)});
    lines.push_back({glm::vec3(0.0f, 3.0f, 0.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f)});
    lines_index.push_back(glm::uvec2(0, 1));
    vao_lines.update_data("vertex", GL_ARRAY_BUFFER, lines.size() * sizeof(float) * 7, lines.data());
    vao_lines.update_data("index", GL_ELEMENT_ARRAY_BUFFER, lines_index.size() * sizeof(uint32_t) * 2, lines_index.data());
    vao_lines.shaders(
            {vertex_shader_text, nullptr, fragment_shader_text},
            {"vPos", "vCol"}, 7, {0, 3}, {"MVP"},
            {[this](GLint loc){glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(this->mvp));}}
    );
}

void Planet::render(int width, int height, float delta) {
    float ratio = float(width) / height;
    p = glm::perspective(45.0f, ratio, 0.01f, 100.0f);
    v = glm::lookAt(glm::vec3(0.0f, 0.0f, -config.camera_distance),
                    glm::vec3(0.0f, 0.0f, 0.0f),
                    glm::vec3(0.0f, 1.0f, 0.0f));
    if(config.rotating) {
        config.rotation_amount += delta / 2;
        m = glm::rotate(glm::mat4(1.0f), config.rotation_amount, config.rotation_axis);
    }
    mvp = p * v * m;

    if (dirty) {
        vao_planet.update_data("vertex", GL_ARRAY_BUFFER, planet.vertices.size() * sizeof(float) * 7, planet.vertices.data());
        vao_planet.update_data("index", GL_ELEMENT_ARRAY_BUFFER, planet.triangles.size() * sizeof(uint32_t) * 3, planet.triangles.data());

        vao_clouds.update_data("vertex", GL_ARRAY_BUFFER, clouds.vertices.size() * sizeof(float) * 7, clouds.vertices.data());
        vao_clouds.update_data("index", GL_ELEMENT_ARRAY_BUFFER, clouds.triangles.size() * sizeof(uint32_t) * 3, clouds.triangles.data());

        lines.clear();
        lines.push_back({-(config.radius + 1) * config.rotation_axis, glm::vec4(1.0f, 1.0f, 0.0f, 1.0f)});
        lines.push_back({(1 + config.radius) * config.rotation_axis, glm::vec4(1.0f, 1.0f, 0.0f, 1.0f)});
        vao_lines.update_data("vertex", GL_ARRAY_BUFFER, lines.size() * sizeof(float) * 7, lines.data());
        dirty = false;
    }
    vao_planet.setup();
    CHECK_GL_ERROR(glDrawElements(GL_TRIANGLES, planet.triangles.size() * 3, GL_UNSIGNED_INT, 0));

    vao_lines.setup();
    CHECK_GL_ERROR(glDrawElements(GL_LINES, lines_index.size() * 2, GL_UNSIGNED_INT, 0));

    if(config.show == config.PLANET) {
        vao_clouds.setup();
        CHECK_GL_ERROR(glDrawElements(GL_TRIANGLES, clouds.triangles.size() * 3, GL_UNSIGNED_INT, 0));
    }
}

void Planet::generate(const Configuration cfg) {
    // Generate the planet
    if(config.seed != cfg.seed || config.show != cfg.show || config.radius != cfg.radius || config.subdivisions != cfg.subdivisions
            || config.rotation_axis != cfg.rotation_axis || config.sea_level != cfg.sea_level) {
        dirty = true;
        planet.regen();
        for(int i = 0; i < cfg.subdivisions; i++) {
            planet.subdivide();
        }

        for(int i = 0; i < planet.vertices.size(); i++) {
            auto vert = planet.vertices[i];
            auto v = vert.vertex;

            // Add 2 to move away from origin. Perlin does not handle negatives well
            float noise = (octave_perlin(v.x/4.0f + 2 + cfg.seed, v.y/4.0f + 2 + cfg.seed, v.z/4.0f + 2 + cfg.seed, 4, 3)
                 + 0.25f * octave_perlin(v.x/4.0f + 2 + cfg.seed, v.y/4.0f + 2 + cfg.seed, v.z/4.0f + 2 + cfg.seed, 6, 3))/1.25f;
            float height = noise * 0.6f - 0.3f + cfg.radius;

            // Noise -> [0,1]
            float heat = (octave_perlin(v.x/2.0f + 2 + cfg.seed + 2 + 2 * cfg.radius, v.y/2.0f + 2 + cfg.seed + 2 + 2 * cfg.radius, v.z/2.0f + 2 + cfg.seed + 2 + 2 * cfg.radius, 4, 3)
                    + 0.25f * octave_perlin(v.x/2.0f + 2 + cfg.seed + 2 + 4 * cfg.radius, v.y/2.0f + 2 + cfg.seed + 2 + 4 * cfg.radius, v.z/2.0f + 2 + cfg.seed + 2 + 4 * cfg.radius, 6, 3))/1.25f
            // Multiply by distance from equator normalized by radius [0,1]
            // Take into consideration the axis
                    // Proj_axis(v) = (v dot axis / |axis|^2) axis
                    // |Proj_axis(v)| = (|v dot axis| / |axis|^2) |axis|
                    // |axis| = 1
                    // 1 - | Proj_axis(v) | because want higher heat at 0 dist
                    * (1 - glm::abs(cfg.rotation_axis.x * v.x + cfg.rotation_axis.y * v.y))
            // Account for height
                    - 0.1f * (height - cfg.radius) / cfg.radius;
            // Noise -> [0,1]
            float moisture = (octave_perlin(v.x/4.0f + 2 + cfg.seed + 6 * cfg.radius, v.y/4.0f + 2 + cfg.seed + 6 * cfg.radius, v.z/4.0f + 2 + cfg.seed + 6 * cfg.radius, 4, 3)
                    + octave_perlin(v.x/4.0f + 2 + cfg.seed + 8 * cfg.radius, v.y/4.0f + 2 + cfg.seed + 8 * cfg.radius, v.z/4.0f + 2 + cfg.seed + 8 * cfg.radius, 6, 3))/4.0f;

            // Bias towards water
            if(noise < 0.475f) {
                moisture = (moisture + (1 - noise))/2.0f;
            }
            int heat_t = int(heat * 8.0f / 0.9f);
            if(heat_t >= 6) heat_t = 5;
            int moisture_t = int(moisture * 9);
            if(moisture_t >= 6) moisture_t = 5;
            planet.vertices[i] = {noise >= 0.0f ? height * v : v * cfg.radius,
                    cfg.show == cfg.PLANET ?
                        noise >= cfg.sea_level ?
                            biomes[moisture_t][heat_t] :
                            glm::vec4(0.0f, 0.0f, noise, 1.0f) :
                    cfg.show == cfg.HEAT ?
                        //glm::vec4(heat, heat, heat, 1.0f) :
                        glm::vec4(heat_t / 5.0f, heat_t / 5.0f, heat_t/5.0f, 1.0f) :
                        //glm::vec4(moisture, moisture, moisture, 1.0f)
                        glm::vec4(moisture_t / 5.0f, moisture_t / 5.0f, moisture_t / 5.0f, 1.0f)
                };
        }

        // Generate clouds
        clouds.regen();
        for(int i = 0; i < cfg.cloud_subdivisions; i++) { // Fewer subdivisions for performance
            clouds.subdivide();
        }

        for(int i = 0; i < clouds.vertices.size(); i++) {
            auto vert = clouds.vertices[i];
            auto v = vert.vertex;
            float noise = octave_perlin(v.x/1.4f + 2, v.y/1.4f + 2, v.z/1.4f + 2, 4, 2);
            noise = noise * 0.6f - 0.3f;
            clouds.vertices[i] = {v * cfg.cloud_radius,
                noise >= 0.0f ?
                    glm::vec4(1.0f, 1.0f, 1.0f, 5 * noise) :
                    glm::vec4(1.0f, 1.0f, 1.0f, 0.0f)};
        }
    }
    if(!dirty) {
        float ra = config.rotation_amount;
        config = cfg;
        config.rotation_amount = ra;
    } else {
        config = cfg;
    }
}
