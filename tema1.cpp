#include "lab_m1/tema1/tema1.h"
#include "lab_m1/tema1/object_2D.h"
#include "lab_m1/tema1/transform_2D.h"

#include <vector>
#include <iostream>

using namespace std;
using namespace m1;


Tema1::Tema1()
{
}


Tema1::~Tema1()
{
}

float Tema1::genRandFloat(float min, float max)
{
    return (min + static_cast<float>(rand()) * static_cast<float>(max - min) / RAND_MAX);
}


void Tema1::Init()
{
    resolution = window->GetResolution();
    auto camera = GetSceneCamera();
    camera->SetOrthographic(0, (float)resolution.x, 0, (float)resolution.y, 0.01f, 400);
    camera->SetPosition(glm::vec3(0, 0, 50));
    camera->SetRotation(glm::vec3(0, 0, 0));
    camera->Update();
    GetCameraInput()->SetActive(false);

    tx = 0;
    ty = 0;

    angStep = 0;

    invRotation = 1;
    invX = 1;
    invY = 1;

    /* Setare dimensiuni componente rata */

    bodyLength = 100;
    bodyScale = glm::vec3(1, 1.5f, 0);

    wingLength = 125;
    wingScale = glm::vec3(0.5f, 0.75f, 0);

    headRadius = 50;
    headScale = glm::vec3(0.5f, 0.5f, 0);

    beakLength = 25;
    beakScale = glm::vec3(0.25f, 0.5f, 0);

    srand((time(0)));
    /* Se genereaza un nr. random intre latimea maxima a ratei si latimea
    ecranului - latimea maxima a ratei, reprezentand punctul de aparitie 
    (de pe ox) al ratei, astfel incat sa se poata vedea rata in intregime
    */
    float maxDuckWidth = bodyLength * bodyScale.x + wingLength * wingScale.x * 2;
    x = genRandFloat(maxDuckWidth, resolution.x - maxDuckWidth);

    float min = M_PI / 36;
    float max = M_PI - min;
    do {
        dirAngle = genRandFloat(min, max);
    } while ((dirAngle >= (M_PI / 2 - min)) && (dirAngle <= (M_PI / 2 + min)));

    /* Calcul puncte de referinta (colt stanga jos sau centru) */

    bodyCorner = glm::vec3(x, 0, 0);
    bodyCenter = object_2D::triangleCenter(bodyCorner, bodyLength, bodyScale);

    headCenter = glm::vec3(
        bodyCorner.x + bodyLength * bodyScale.x / 2, bodyCorner.y + bodyLength * bodyScale.y, 0);

    beakCorner = glm::vec3(headCenter.x, headCenter.y + headRadius * headScale.y, 0);
    beakCenter = object_2D::triangleCenter(beakCorner, beakLength, beakScale);


    Mesh* body = object_2D::CreateTriangle(
                "body", glm::vec3(0, 0, 0), bodyLength, glm::vec3(0.3f, 0.1f, 0), true);
    AddMeshToList(body);

    Mesh* wing = object_2D::CreateTriangle(
                "wing", glm::vec3(0, 0, 0), wingLength, glm::vec3(0.4f, 0.1f, 0.1f), true);
    AddMeshToList(wing);

    Mesh* head = object_2D::CreateCircle(
                "head", glm::vec3(0, 0, 0), headRadius, glm::vec3(0.1f, 0.3f, 0), true);
    AddMeshToList(head);

    Mesh* beak = object_2D::CreateTriangle(
                "beak", glm::vec3(0, 0, 0), beakLength, glm::vec3(0.9f, 0.6f, 0), true);
    AddMeshToList(beak);
}


void Tema1::FrameStart()
{
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    glViewport(0, 0, resolution.x, resolution.y);
}


void Tema1::Update(float deltaTimeSeconds)
{
    parentMatrix = glm::mat3(1);

    tx += cos(dirAngle) * deltaTimeSeconds * 200;
    ty += sin(dirAngle) * deltaTimeSeconds * 200;

    angStep += invRotation * deltaTimeSeconds;

    // cout << "res = " << resolution.x << ", " << resolution.y << "\n";
    // cout << "tx = " << (x + (tx)) << "\n";
    // cout << "ty = " << ((ty)) << "\n";
    
    // perete dreapta
    if ((bodyCenter.x + tx) >= resolution.x) {
        if (sin(dirAngle) > 0) {
            dirAngle = (M_PI) - dirAngle;
        } else {
            dirAngle = (M_PI) + (2 * M_PI - dirAngle);
        }
    }
    // perete stanga
    if ((bodyCenter.x + tx) <= 0) {
        if (sin(dirAngle) > 0) {
            dirAngle = (M_PI) - dirAngle;
        } else {
            dirAngle = (3 * M_PI / 2) + (3 * M_PI / 2 - dirAngle);
        }
    }

    // perete sus
    if (bodyCenter.y + ty >= resolution.y) {
        if (cos(dirAngle) > 0) {
            dirAngle = (2 * M_PI) - dirAngle;
        } else {
            dirAngle = (2 * M_PI) - dirAngle;
        }
    }
    // perete jos
    if (bodyCenter.y + ty <= 0) {
        if (cos(dirAngle) > 0) {
            dirAngle = (2 * M_PI) - dirAngle;
        } else {
            dirAngle = (2 * M_PI) - dirAngle;
        }
    }

    if (angStep >= 0.5f) {
        invRotation = -1;
    }
    if (angStep <= 0) {
        invRotation = 1;
    }

    parentMatrix *= transform_2D::Translate(tx, ty);

    // Ciocul ratei

    modelMatrix = glm::mat3(1);
    modelMatrix *= transform_2D::Translate(beakCorner.x, beakCorner.y);

    modelMatrix *= parentMatrix;

    modelMatrix *= transform_2D::Translate(bodyCenter.x - beakCorner.x, bodyCenter.y - beakCorner.y);
    modelMatrix *= transform_2D::Rotate(-M_PI / 2 + dirAngle);
    modelMatrix *= transform_2D::Translate(-bodyCenter.x + beakCorner.x, -bodyCenter.y + beakCorner.y);

    modelMatrix *= transform_2D::Rotate(-M_PI / 36);
    modelMatrix *= transform_2D::Scale(beakScale.x, beakScale.y);

    RenderMesh2D(meshes["beak"], shaders["VertexColor"], modelMatrix);

    // Capul ratei

    modelMatrix = glm::mat3(1);
    modelMatrix *= transform_2D::Translate(headCenter.x, headCenter.y);

    modelMatrix *= parentMatrix;

    modelMatrix *= transform_2D::Translate(bodyCenter.x - headCenter.x, bodyCenter.y - headCenter.y);
    modelMatrix *= transform_2D::Rotate(-M_PI / 2 + dirAngle);
    modelMatrix *= transform_2D::Translate(-bodyCenter.x + headCenter.x, -bodyCenter.y + headCenter.y);

    modelMatrix *= transform_2D::Scale(headScale.x, headScale.y);

    RenderMesh2D(meshes["head"], shaders["VertexColor"], modelMatrix);

    // Corpul ratei

    modelMatrix = glm::mat3(1);
    modelMatrix *= transform_2D::Translate(bodyCorner.x, bodyCorner.y);

    modelMatrix *= parentMatrix;

    modelMatrix *= transform_2D::Translate(bodyCenter.x - bodyCorner.x, bodyCenter.y - bodyCorner.y);
    modelMatrix *= transform_2D::Rotate(-M_PI / 2 + dirAngle);
    modelMatrix *= transform_2D::Translate(-bodyCenter.x + bodyCorner.x, -bodyCenter.y + bodyCorner.y);

    modelMatrix *= transform_2D::Scale(bodyScale.x, bodyScale.y);

    RenderMesh2D(meshes["body"], shaders["VertexColor"], modelMatrix);

    // Aripa stanga

    wingCorner = glm::vec3(bodyCenter.x - wingLength * wingScale.x, bodyCenter.y, 0);

    modelMatrix = glm::mat3(1);
    modelMatrix *= transform_2D::Translate(wingCorner.x, wingCorner.y);

    modelMatrix *= parentMatrix;

    modelMatrix *= transform_2D::Translate(bodyCenter.x - wingCorner.x, bodyCenter.y - wingCorner.y);
    modelMatrix *= transform_2D::Rotate(-M_PI / 2 + dirAngle);
    modelMatrix *= transform_2D::Translate(-bodyCenter.x + wingCorner.x, -bodyCenter.y + wingCorner.y);

    wingCenter = object_2D::triangleCenter(wingCorner, wingLength, wingScale);

    modelMatrix *= transform_2D::Translate(wingCenter.x - wingCorner.x, wingCenter.y - wingCorner.y);
    modelMatrix *= transform_2D::Rotate(M_PI / 2);
    modelMatrix *= transform_2D::Translate(-wingCenter.x + wingCorner.x, -wingCenter.y + wingCorner.y);

    modelMatrix *= transform_2D::Translate(wingCenter.x - wingCorner.x, wingCenter.y - wingCorner.y);
    modelMatrix *= transform_2D::Rotate(angStep);
    modelMatrix *= transform_2D::Translate(-wingCenter.x + wingCorner.x, -wingCenter.y + wingCorner.y);

    modelMatrix *= transform_2D::Scale(wingScale.x, wingScale.y);

    RenderMesh2D(meshes["wing"], shaders["VertexColor"], modelMatrix);

    // Aripa dreapta

    wingCorner = glm::vec3(bodyCenter.x, bodyCenter.y, 0);

    modelMatrix = glm::mat3(1);
    modelMatrix *= transform_2D::Translate(wingCorner.x, wingCorner.y);

    modelMatrix *= parentMatrix;

    modelMatrix *= transform_2D::Translate(bodyCenter.x - wingCorner.x, bodyCenter.y - wingCorner.y);
    modelMatrix *= transform_2D::Rotate(-M_PI / 2 + dirAngle);
    modelMatrix *= transform_2D::Translate(-bodyCenter.x + wingCorner.x, -bodyCenter.y + wingCorner.y);

    wingCenter = object_2D::triangleCenter(wingCorner, wingLength, wingScale);

    modelMatrix *= transform_2D::Translate(wingCenter.x - wingCorner.x, wingCenter.y - wingCorner.y);
    modelMatrix *= transform_2D::Rotate(-M_PI / 2);
    modelMatrix *= transform_2D::Translate(-wingCenter.x + wingCorner.x, -wingCenter.y + wingCorner.y);

    modelMatrix *= transform_2D::Translate(wingCenter.x - wingCorner.x, wingCenter.y - wingCorner.y);
    modelMatrix *= transform_2D::Rotate(-angStep);
    modelMatrix *= transform_2D::Translate(-wingCenter.x + wingCorner.x, -wingCenter.y + wingCorner.y);

    modelMatrix *= transform_2D::Scale(wingScale.x, wingScale.y);

    RenderMesh2D(meshes["wing"], shaders["VertexColor"], modelMatrix);
}


void Tema1::FrameEnd()
{
}


void Tema1::OnInputUpdate(float deltaTime, int mods)
{
}


void Tema1::OnKeyPress(int key, int mods)
{
}


void Tema1::OnKeyRelease(int key, int mods)
{
}


void Tema1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
}


void Tema1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
}


void Tema1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
}


void Tema1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Tema1::OnWindowResize(int width, int height)
{
}
