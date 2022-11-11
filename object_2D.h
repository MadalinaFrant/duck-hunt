#pragma once

#include <string>

#include "core/gpu/mesh.h"
#include "utils/glm_utils.h"


namespace object_2D
{
    // Creeaza un patrat cu coltul de stanga jos, lungimea si culoarea date
    Mesh* CreateSquare(const std::string &name, glm::vec3 leftBottomCorner, 
                        float length, glm::vec3 color, bool fill = false);

    // Creeaza un triunghi cu coltul de stanga jos, lungimea si culoarea date
    Mesh* CreateTriangle(const std::string &name, glm::vec3 leftBottomCorner, 
                        float length, glm::vec3 color, bool fill = false);

    // Creeaza un cerc cu centrul, raza si culoarea date
    Mesh* CreateCircle(const std::string &name, glm::vec3 center, 
                        float radius, glm::vec3 color, bool fill = false);

    // Intoarce centrul triunghiului
    glm::vec3 TriangleCenter(glm::vec3 leftBottomCorner, float length, glm::vec3 scale);

    // Intoarce centrul patratului/dreptunghiului
    glm::vec3 SquareCenter(glm::vec3 leftBottomCorner, float length, glm::vec3 scale);

}
