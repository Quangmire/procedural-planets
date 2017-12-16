#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <glm/glm.hpp>


struct Configuration {
    int PLANET   = 0;
    int HEAT     = 1;
    int MOISTURE = 2;
    glm::vec3 rotation_axis = glm::vec3(0.0f, 1.0f, 0.0f);
    float rotation_amount = 0.0f;
    bool rotating = true;
    float radius = 2.0f;
    float cloud_radius = 2.13f;
    float camera_distance = 5.0f;
    int subdivisions = 7;
    int cloud_subdivisions = 6;
    int show = PLANET;
    float seed = 0;
    float sea_level = 0.475f;
};

#endif
