#include "object_2D.h"

#include <vector>

#include "core/engine.h"
#include "utils/gl_utils.h"


Mesh* object_2D::CreateSquare(
    const std::string &name,
    glm::vec3 leftBottomCorner,
    float length,
    glm::vec3 color,
    bool fill)
{
    glm::vec3 corner = leftBottomCorner;

    std::vector<VertexFormat> vertices =
    {
        VertexFormat(corner, color),
        VertexFormat(corner + glm::vec3(length, 0, 0), color),
        VertexFormat(corner + glm::vec3(length, length, 0), color),
        VertexFormat(corner + glm::vec3(0, length, 0), color)
    };

    std::vector<unsigned int> indices = { 0, 1, 2, 3 };

    Mesh* square = new Mesh(name);

    if (!fill) {
        square->SetDrawMode(GL_LINE_LOOP);
    } else {
        indices.push_back(0);
        indices.push_back(2);
    }

    square->InitFromData(vertices, indices);
    return square;
}

Mesh* object_2D::CreateTriangle(
    const std::string &name,
    glm::vec3 leftBottomCorner,
    float length,
    glm::vec3 color,
    bool fill)
{
    glm::vec3 corner = leftBottomCorner;

    std::vector<VertexFormat> vertices =
    {
        VertexFormat(corner, color),
        VertexFormat(corner + glm::vec3(length, 0, 0), color),
        VertexFormat(corner + glm::vec3(length / 2, length, 0), color)
    };

    std::vector<unsigned int> indices = { 0, 1, 2 };

    Mesh* triangle = new Mesh(name);

    if (!fill) {
        triangle->SetDrawMode(GL_LINE_LOOP);
    }

    triangle->InitFromData(vertices, indices);
    return triangle;
}

Mesh* object_2D::CreateCircle(
    const std::string &name,
    glm::vec3 center,
    float radius,
    glm::vec3 color,
    bool fill)
{
    std::vector<VertexFormat> vertices;
    vertices.push_back(VertexFormat(center, color));

    float inc = 2.0f * M_PI / 360;

    for (float angle = 0.0f; angle <= 2.0f * M_PI; angle += inc)
    {
        vertices.push_back(VertexFormat(
            glm::vec3(radius * cos(angle) + center.x, radius * sin(angle) + center.y, 0), 
            color));
    }

    std::vector<unsigned int> indices;

    Mesh* circle = new Mesh(name);

    if (!fill) {

        circle->SetDrawMode(GL_LINE_LOOP);

        for (int i = 1; i < vertices.size() - 1; i += 2) {
            indices.push_back(i);
            indices.push_back(i + 1);
        }

        indices.push_back(vertices.size() - 1);
        indices.push_back(1);

    } else {

        for (int i = 1; i < vertices.size() - 1; i++) {
            indices.push_back(i);
            indices.push_back(0);
            indices.push_back(i + 1);
        }

        indices.push_back(vertices.size() - 1);
        indices.push_back(0);
        indices.push_back(1);
    }

    circle->InitFromData(vertices, indices);
    return circle;
}

glm::vec3 object_2D::triangleCenter(glm::vec3 leftBottomCorner, float length, glm::vec3 scale) 
{
    float x1 = leftBottomCorner.x;
    float x2 = leftBottomCorner.x + length * scale.x;
    float x3 = leftBottomCorner.x + length * scale.x / 2;

    float y1 = leftBottomCorner.y;
    float y2 = leftBottomCorner.y;
    float y3 = leftBottomCorner.y + length * scale.y;

    glm::vec3 center;
    center.x = (x1 + x2 + x3) / 3;
    center.y = (y1 + y2 + y3) / 3;
    center.z = 0;

    return center;
}

