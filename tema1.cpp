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


void Tema1::Init()
{
    resolution = window->GetResolution();
    auto camera = GetSceneCamera();
    camera->SetOrthographic(0, (float)resolution.x, 0, (float)resolution.y, 0.01f, 400);
    camera->SetPosition(glm::vec3(0, 0, 50));
    camera->SetRotation(glm::vec3(0, 0, 0));
    camera->Update();
    GetCameraInput()->SetActive(false);

    currState = "newDuck";

    lives = 3;

    speedInc = 1;

    /* Setare dimensiuni componente rata */

    bodyLength = 100;
    bodyScale = glm::vec3(1, 1.5f, 0);

    wingLength = 125;
    wingScale = glm::vec3(0.5f, 0.75f, 0);

    headRadius = 50;
    headScale = glm::vec3(0.5f, 0.5f, 0);

    beakLength = 25;
    beakScale = glm::vec3(0.25f, 0.5f, 0);

    eyeRadius = 20;
    eyeScale = glm::vec3(0.25f, 0.25f, 0);

    lineLength = 10;
    lineScale = glm::vec3(0.25f, 1.25f, 0);

    grassScale = glm::vec3(resolution.x, resolution.y / 5, 0);


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

    Mesh* eye = object_2D::CreateCircle(
                "eye", glm::vec3(0, 0, 0), eyeRadius, glm::vec3(1, 1, 1), true);
    AddMeshToList(eye);

    Mesh* line = object_2D::CreateSquare(
                "line", glm::vec3(0, 0, 0), lineLength, glm::vec3(1, 1, 1), true);
    AddMeshToList(line);

    Mesh* grass = object_2D::CreateSquare(
                "grass", glm::vec3(0, 0, 0), 1, glm::vec3(0, 0.5f, 0.2f), true);
    AddMeshToList(grass);
}


void Tema1::FrameStart()
{
    if (currState == "evaded") {
        glClearColor(0.7f, 0.3f, 0.4f, 1);
    } else {
        glClearColor(0.4f, 0.7, 1, 1);
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    glViewport(0, 0, resolution.x, resolution.y);
}


void Tema1::Update(float deltaTimeSeconds)
{
    currDuckTime += deltaTimeSeconds;

    if (currDuckTime >= 5) {
        currState = "evaded";
    }

    if ((currState == "evaded") && (bodyCorner.y + ty >= resolution.y)) {
        currState = "newDuck";
    }
    if ((currState == "shot") && (bodyCorner.y + ty <= grassScale.y)) {
        currState = "newDuck";
    }

    if (currState == "newDuck") {
        CalculateValues();
        currState = "active";
    } else if (currState == "evaded") {
        dirAngle = M_PI / 2;
    } else if (currState == "shot") {
        dirAngle = -M_PI / 2;
    } else { // currState == "active"
        // Calculul noului unghi dupa reflexie
        CalculateDirAngle();
    }

    tx += cos(dirAngle) * deltaTimeSeconds * speed;
    ty += sin(dirAngle) * deltaTimeSeconds * speed;

    // Schimbare sens aripi
    if (wingRotationAngle >= 0.5f) {
        wingRotationSense = -1;
    }
    if (wingRotationAngle <= 0) {
        wingRotationSense = 1;
    }

    wingRotationAngle += wingRotationSense * deltaTimeSeconds;

    RenderGrass();
    RenderDuck();
}


float Tema1::GenRandFloat(float min, float max)
{
    return (min + static_cast<float>(rand()) * static_cast<float>(max - min) / RAND_MAX);
}


void Tema1::CalculateValues()
{
    currDuckTime = 0;

    bullets = 3;

    speed = 250 * speedInc;

    tx = 0;
    ty = 0;

    wingRotationAngle = 0;
    wingRotationSense = 1;

    srand((time(0)));
    /* Se genereaza un nr. random intre latimea maxima a ratei si latimea
    ecranului - latimea maxima a ratei, reprezentand punctul de aparitie 
    (de pe ox) al ratei, astfel incat sa se poata vedea rata in intregime
    */
    float maxDuckWidth = bodyLength * bodyScale.x + wingLength * wingScale.x * 2;
    spawnPoint = glm::vec3(GenRandFloat(maxDuckWidth, resolution.x - maxDuckWidth), 0, 0);

    float min = M_PI / 36;
    float max = M_PI - min;
    do {
        dirAngle = GenRandFloat(min, max);
    } while ((dirAngle >= (M_PI / 2 - min)) && (dirAngle <= (M_PI / 2 + min)));

    /* Calcul puncte de referinta (colt stanga jos sau centru) */

    bodyCorner = spawnPoint;
    bodyCenter = object_2D::TriangleCenter(bodyCorner, bodyLength, bodyScale);

    headCenter = glm::vec3(
        bodyCorner.x + bodyLength * bodyScale.x / 2, bodyCorner.y + bodyLength * bodyScale.y, 0);

    beakCorner = glm::vec3(headCenter.x, headCenter.y + headRadius * headScale.y, 0);
    beakCenter = object_2D::TriangleCenter(beakCorner, beakLength, beakScale);

    eyeCenter = glm::vec3(headCenter.x + headRadius * headScale.x / 3, headCenter.y + headRadius * headScale.y / 3, 0);

    lineCorner = headCenter;
    lineCenter = object_2D::SquareCenter(lineCorner, lineLength, lineScale);
}


void Tema1::CalculateDirAngle() 
{
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
}


void Tema1::RotateInRefToPoint(glm::vec3 point, glm::vec3 refPoint, float angle) 
{
    modelMatrix *= transform_2D::Translate(refPoint.x - point.x, refPoint.y - point.y);
    modelMatrix *= transform_2D::Rotate(angle);
    modelMatrix *= transform_2D::Translate(-refPoint.x + point.x, -refPoint.y + point.y);
}


void Tema1::RenderDuck()
{
    // Miscarea ratei
    parentMatrix = glm::mat3(1);
    parentMatrix *= transform_2D::Translate(tx, ty);

    // Ciocul ratei
    RenderBeak();

    if (currState == "shot") {
        RenderCross();
    } else {
        RenderEye();
    }

    // Capul ratei
    RenderHead();

    // Corpul ratei
    RenderBody();

    // Aripa stanga
    wingCorner = glm::vec3(bodyCenter.x - wingLength * wingScale.x, bodyCenter.y, 0);
    RenderWing(1);

    // Aripa dreapta
    wingCorner = glm::vec3(bodyCenter.x, bodyCenter.y, 0);
    RenderWing(-1);
}


void Tema1::RenderEye() 
{
    modelMatrix = glm::mat3(1);
    modelMatrix *= transform_2D::Translate(eyeCenter.x, eyeCenter.y);

    modelMatrix *= parentMatrix;

    // Rotatie in jurul centrului corpului, pentru rotatia in functie de directie
    RotateInRefToPoint(eyeCenter, bodyCenter, (-M_PI / 2 + dirAngle));
    
    modelMatrix *= transform_2D::Scale(eyeScale.x, eyeScale.y);

    RenderMesh2D(meshes["eye"], shaders["VertexColor"], modelMatrix);
}


void Tema1::RenderLine(int sense) 
{
    modelMatrix = glm::mat3(1);
    modelMatrix *= transform_2D::Translate(lineCorner.x, lineCorner.y);

    modelMatrix *= parentMatrix;

    // Rotatie in jurul centrului corpului, pentru rotatia in functie de directie
    RotateInRefToPoint(lineCorner, bodyCenter, (-M_PI / 2 + dirAngle));

    // Rotatie cu scopul pozitionarii estetice a liniei
    RotateInRefToPoint(lineCorner, lineCenter, sense * M_PI / 4);
    modelMatrix *= transform_2D::Scale(lineScale.x, lineScale.y);

    RenderMesh2D(meshes["line"], shaders["VertexColor"], modelMatrix);
}


void Tema1::RenderCross() 
{
    RenderLine(1);
    RenderLine(-1);
}


void Tema1::RenderBeak() 
{
    modelMatrix = glm::mat3(1);
    modelMatrix *= transform_2D::Translate(beakCorner.x, beakCorner.y);

    modelMatrix *= parentMatrix;

    // Rotatie in jurul centrului corpului, pentru rotatia in functie de directie
    RotateInRefToPoint(beakCorner, bodyCenter, (-M_PI / 2 + dirAngle));

    // Rotatie cu scopul pozitionarii estetice a ciocului fata de restul ratei
    modelMatrix *= transform_2D::Rotate(-M_PI / 36);
    modelMatrix *= transform_2D::Scale(beakScale.x, beakScale.y);

    RenderMesh2D(meshes["beak"], shaders["VertexColor"], modelMatrix);
}


void Tema1::RenderHead() 
{
    modelMatrix = glm::mat3(1);
    modelMatrix *= transform_2D::Translate(headCenter.x, headCenter.y);

    modelMatrix *= parentMatrix;

    // Rotatie in jurul centrului corpului, pentru rotatia in functie de directie
    RotateInRefToPoint(headCenter, bodyCenter, (-M_PI / 2 + dirAngle));

    modelMatrix *= transform_2D::Scale(headScale.x, headScale.y);

    RenderMesh2D(meshes["head"], shaders["VertexColor"], modelMatrix);
}


void Tema1::RenderBody() 
{
    modelMatrix = glm::mat3(1);
    modelMatrix *= transform_2D::Translate(bodyCorner.x, bodyCorner.y);

    modelMatrix *= parentMatrix;

    // Rotatie in jurul centrului corpului, pentru rotatia in functie de directie
    RotateInRefToPoint(bodyCorner, bodyCenter, (-M_PI / 2 + dirAngle));

    modelMatrix *= transform_2D::Scale(bodyScale.x, bodyScale.y);

    RenderMesh2D(meshes["body"], shaders["VertexColor"], modelMatrix);
}


void Tema1::RenderWing(int sense) 
{
    modelMatrix = glm::mat3(1);
    modelMatrix *= transform_2D::Translate(wingCorner.x, wingCorner.y);

    modelMatrix *= parentMatrix;

    // Rotatie in jurul centrului corpului, pentru rotatia in functie de directie
    RotateInRefToPoint(wingCorner, bodyCenter, (-M_PI / 2 + dirAngle));

    wingCenter = object_2D::TriangleCenter(wingCorner, wingLength, wingScale);

    // Rotatia aripii pentru pozitia initiala
    RotateInRefToPoint(wingCorner, wingCenter, (sense * M_PI / 2));

    // Rotatia aripii in jurul centrului sau, in sensul corespunzator
    RotateInRefToPoint(wingCorner, wingCenter, (sense * wingRotationAngle));

    modelMatrix *= transform_2D::Scale(wingScale.x, wingScale.y);

    RenderMesh2D(meshes["wing"], shaders["VertexColor"], modelMatrix);
}


void Tema1::RenderGrass() 
{
    modelMatrix = glm::mat3(1);
    modelMatrix *= transform_2D::Scale(grassScale.x, grassScale.y);

    RenderMesh2D(meshes["grass"], shaders["VertexColor"], modelMatrix);
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
