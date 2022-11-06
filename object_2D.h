#pragma once

#include <string>

#include "core/gpu/mesh.h"
#include "utils/glm_utils.h"


namespace object_2D
{

    // Create square with given bottom left corner, length and color
    Mesh* CreateSquare(const std::string &name, glm::vec3 leftBottomCorner, 
                        float length, glm::vec3 color, bool fill = false);

    // Create triangle with given bottom left corner, length and color
    Mesh* CreateTriangle(const std::string &name, glm::vec3 leftBottomCorner, 
                        float length, glm::vec3 color, bool fill = false);

    // Create circle with given center, radius and color
    Mesh* CreateCircle(const std::string &name, glm::vec3 center, 
                        float radius, glm::vec3 color, bool fill = false);

    glm::vec3 TriangleCenter(glm::vec3 leftBottomCorner, float length, glm::vec3 scale);

    glm::vec3 SquareCenter(glm::vec3 leftBottomCorner, float length, glm::vec3 scale);

}
