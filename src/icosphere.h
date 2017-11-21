#ifndef ICOSPHERE_H
#define ICOSPHERE_H

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>

struct Icosphere {
private:
    const float a = 0.525731112119133606f;
    const float b = 0.850650808352039932f;
    const float c = 0.0f;
public:
    std::vector<glm::vec3>  vertices;
    std::vector<glm::uvec3> triangles;
    Icosphere() {
        // Add vertices of unit icosphere
        vertices.push_back(glm::vec3(-a,  c,  b));
        vertices.push_back(glm::vec3( a,  c,  b));
        vertices.push_back(glm::vec3(-a,  c, -b));
        vertices.push_back(glm::vec3( a,  c, -b));
        vertices.push_back(glm::vec3( c,  b,  a));
        vertices.push_back(glm::vec3( c,  b, -a));
        vertices.push_back(glm::vec3( c, -b,  a));
        vertices.push_back(glm::vec3( c, -b, -a));
        vertices.push_back(glm::vec3( b,  a,  c));
        vertices.push_back(glm::vec3(-b,  a,  c));
        vertices.push_back(glm::vec3( b, -a,  c));
        vertices.push_back(glm::vec3(-b, -a,  c));
 
        // Add triangles of unit icosphere
        triangles.push_back(glm::uvec3(0,4,1));
        triangles.push_back(glm::uvec3(0,9,4));
        triangles.push_back(glm::uvec3(9,5,4));
        triangles.push_back(glm::uvec3(4,5,8));
        triangles.push_back(glm::uvec3(4,8,1));
        triangles.push_back(glm::uvec3(8,10,1));
        triangles.push_back(glm::uvec3(8,3,10));
        triangles.push_back(glm::uvec3(5,3,8));
        triangles.push_back(glm::uvec3(5,2,3));
        triangles.push_back(glm::uvec3(2,7,3));
        triangles.push_back(glm::uvec3(7,10,3));
        triangles.push_back(glm::uvec3(7,6,10));
        triangles.push_back(glm::uvec3(7,11,6));
        triangles.push_back(glm::uvec3(11,0,6));
        triangles.push_back(glm::uvec3(0,1,6));
        triangles.push_back(glm::uvec3(6,1,10));
        triangles.push_back(glm::uvec3(9,0,11));
        triangles.push_back(glm::uvec3(9,11,2));
        triangles.push_back(glm::uvec3(9,2,5));
        triangles.push_back(glm::uvec3(7,2,11));
    }

    void subdivide() {
        std::vector<glm::uvec3> new_tris;
        // Loop through every triangle
        for(auto iter = triangles.begin(); iter != triangles.end(); iter++) {
            glm::uvec3 tri = *iter;
            uint32_t mid[3];
            // Loop through every edge
            for(uint32_t i = 0; i < 3; i++) {
                // Add normalized midpoints
                glm::vec3 a = vertices[tri[i]];
                glm::vec3 b = vertices[tri[(i + 1) % 3]];
                vertices.push_back(glm::normalize(a + b));
                mid[i] = vertices.size() - 1;
            }
            // t[0] m[0] t[1] m[1] t[2] m[2] t[0]
            new_tris.push_back(glm::uvec3(tri[0], mid[2], mid[0]));
            new_tris.push_back(glm::uvec3(tri[1], mid[0], mid[1]));
            new_tris.push_back(glm::uvec3(tri[2], mid[1], mid[2]));
            new_tris.push_back(glm::uvec3(mid[2], mid[1], mid[0]));
        }
        triangles.clear();
        triangles = new_tris;
    }
};
#endif
