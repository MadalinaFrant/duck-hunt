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
    nrDucks = 0;
    lives = 3;
    initSpeed = 350;
    speedInc = 0;
    scoreInc = 1;

    /* Setare dimensiuni componente rata */

    bodyLength = 100;
    wingLength = 125;
    headRadius = 50;
    beakLength = 25;
    eyeRadius = 20;
    lineLength = 10;

    grassLength = 1;
    grassScale = glm::vec3(resolution.x, (resolution.y / 5), 0);

    lifeRadius = 10;
    lifeScale = glm::vec3(1, 1, 0);

    bulletLength = 10;
    bulletScale = glm::vec3(1, 2.5f, 0);

    score = 0;
    maxScore = 250;
    scoreScale = glm::vec3(1, 0.25f, 0);

    DefineObjects();
}


void Tema1::FrameStart()
{
    /* Cer albastru, care devine roz daca rata a evadat */
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
    currDuckTime += deltaTimeSeconds; // timpul pe ecran al ratei curente

    /* Rata evadeaza daca a stat mai mult de 5 secunde pe ecran sau daca nu 
    a fost nimerita cu niciun glont */
    if (currState == "active") {
        if ((currDuckTime >= 5) || (bullets == 0)) {
            currState = "evaded";
            lives--;
        }
    }

    // Se va genera o noua rata dupa iesirea din ecran a ratei precedente
    if ((currState == "evaded") && ((bodyCorner.y + ty) >= resolution.y)) {
        if (lives == 0) { // s-au epuizat toate vietile
            exit(0);
        }
        currState = "newDuck";
    }
    if ((currState == "shot") && ((bodyCorner.y + ty) <= 0)) {
        if (score == maxScore) { // s-a atins scorul maxim
            exit(0);
        }
        currState = "newDuck";
    }

    if (currState == "newDuck") {
        CalculateValues(); // calcul si initializare valori necesare
        currState = "active";
    } else if (currState == "evaded") {
        dirAngle = M_PI / 2; // miscare in sus
    } else if (currState == "shot") {
        dirAngle = -M_PI / 2; // miscare in jos
        wingRotationAngle = 0; // oprire rotatii aripi cand rata moare
    } else {
        CalculateDirAngle(); // calculul noului unghi dupa reflexie
    }

    // Calcul valori de translatie pentru miscarea ratei pe directia corespunzatoare
    tx += cos(dirAngle) * speed * deltaTimeSeconds;
    ty += sin(dirAngle) * speed * deltaTimeSeconds;

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
    RenderLives();
    RenderBullets();
    RenderScore();
}


void Tema1::DefineObjects()
{
    Mesh* body = object_2D::CreateTriangle(
                "body", glm::vec3(0, 0, 0), bodyLength, glm::vec3(0.3f, 0.1f, 0), true);
    AddMeshToList(body);

    Mesh* wingBlue = object_2D::CreateTriangle(
                "wingBlue", glm::vec3(0, 0, 0), wingLength, glm::vec3(0, 0.2f, 0.6f), true);
    AddMeshToList(wingBlue);

    Mesh* wingBrown = object_2D::CreateTriangle(
                "wingBrown", glm::vec3(0, 0, 0), wingLength, glm::vec3(0.4f, 0.1f, 0.1f), true);
    AddMeshToList(wingBrown);

    Mesh* head = object_2D::CreateCircle(
                "head", glm::vec3(0, 0, 0), headRadius, glm::vec3(0.1f, 0.3f, 0), true);
    AddMeshToList(head);

    Mesh* beak = object_2D::CreateTriangle(
                "beak", glm::vec3(0, 0, 0), beakLength, glm::vec3(0.9f, 0.6f, 0), true);
    AddMeshToList(beak);

    Mesh* eye = object_2D::CreateCircle(
                "eye", glm::vec3(0, 0, 0), eyeRadius, glm::vec3(1, 1, 1), true);
    AddMeshToList(eye);

    Mesh* pupil = object_2D::CreateCircle(
                "pupil", glm::vec3(0, 0, 0), (eyeRadius * 0.75f), glm::vec3(0, 0, 0), true);
    AddMeshToList(pupil);

    Mesh* line = object_2D::CreateSquare(
                "line", glm::vec3(0, 0, 0), lineLength, glm::vec3(1, 1, 1), true);
    AddMeshToList(line);

    Mesh* grass = object_2D::CreateSquare(
                "grass", glm::vec3(0, 0, 0), grassLength, glm::vec3(0, 0.5f, 0.2f), true);
    AddMeshToList(grass);

    Mesh* life = object_2D::CreateCircle(
                "life", glm::vec3(0, 0, 0), lifeRadius, glm::vec3(1, 0, 0), true);
    AddMeshToList(life);

    Mesh* bullet = object_2D::CreateSquare(
                "bullet", glm::vec3(0, 0, 0), bulletLength, glm::vec3(0.8f, 0.6f, 0), true);
    AddMeshToList(bullet);

    Mesh* scoreBox = object_2D::CreateSquare(
                "scoreBox", glm::vec3(0, 0, 0), maxScore, glm::vec3(0, 0, 1), false);
    AddMeshToList(scoreBox);

    Mesh* currScore = object_2D::CreateSquare(
                "currScore", glm::vec3(0, 0, 0), maxScore, glm::vec3(0, 0, 1), true);
    AddMeshToList(currScore);
}


float Tema1::GenRandFloat(float min, float max)
{
    srand((time(0)));
    return (min + static_cast<float>(rand()) * static_cast<float>(max - min) / RAND_MAX);
}


void Tema1::CalculateValues()
{
    nrDucks++;

    /* Fiecare a 10-a rata va fi boss, care va fi mai mare, se va misca mai incet,
    si va necesita 3 gloante pentru a fi omorata */
    if (nrDucks % 10 == 0) {
        bossFight = true;
        bullets = 5;

        bodyScale = glm::vec3(bodyScale.x * 1.5f, bodyScale.y * 1.5f, 0);
        wingScale = glm::vec3(wingScale.x * 1.5f, wingScale.y * 1.5f, 0);
        headScale = glm::vec3(headScale.x * 1.5f, headScale.y * 1.5f, 0);
        beakScale = glm::vec3(beakScale.x * 1.5f, beakScale.y * 1.5f, 0);
        eyeScale = glm::vec3(eyeScale.x * 1.5f, eyeScale.y * 1.5f, 0);
        lineScale = glm::vec3(lineScale.x * 1.5f, lineScale.y * 1.5f, 0);

        speed = initSpeed - (initSpeed / 5);
        scorePerDuck = 10 * scoreInc;
    } else {
        bossFight = false;
        bullets = 3;

        bodyScale = glm::vec3(1, 1.5f, 0);
        wingScale = glm::vec3(0.5f, 0.75f, 0);
        headScale = glm::vec3(0.5f, 0.5f, 0);
        beakScale = glm::vec3(0.25f, 0.5f, 0);
        eyeScale = glm::vec3(0.25f, 0.25f, 0);
        lineScale = glm::vec3(0.25f, 1.25f, 0);

        speed = initSpeed + speedInc * (initSpeed / 5);
        scorePerDuck = 5 * scoreInc;
    }

    // Dupa cate 5 rate va creste viteza acestora si scorul adus de o rata
    if (nrDucks % 5 == 0) {
        speedInc++;
        scoreInc++;
    }

    bulletsShot = 0; // numarul de gloante nimerite

    currDuckTime = 0;

    tx = 0;
    ty = 0;

    wingRotationAngle = 0;
    wingRotationSense = 1;

    duckHeight = (bodyLength * bodyScale.y) + (headRadius * headScale.y) + (beakLength * beakScale.y);
    duckWidth = (bodyLength * bodyScale.x) + (wingLength * wingScale.y) * 2;

    /* Se genereaza un nr. random intre latimea maxima a ratei si latimea
    ecranului - latimea maxima a ratei, reprezentand punctul de aparitie 
    (de pe ox) al ratei, astfel incat sa se poata vedea rata in intregime */
    spawnPoint = glm::vec3(GenRandFloat(duckWidth, (resolution.x - duckWidth)), 0, 0);

    /* Se genereaza un unghi random pentru a reprezenta directia, exceptand
    valori prea apropiate de axa ox (unghi de 0 sau 180 de grade) sau de axa oy
    (unghi de 90 de grade) */
    float min = M_PI / 36;
    float max = M_PI - min;
    do {
        dirAngle = GenRandFloat(min, max);
    } while ((dirAngle >= (M_PI / 2 - min)) && (dirAngle <= (M_PI / 2 + min)));

    /* Calcul puncte de referinta (colt stanga jos sau centru) */

    bodyCorner = spawnPoint;
    bodyCenter = object_2D::TriangleCenter(bodyCorner, bodyLength, bodyScale);

    headCenter = glm::vec3(
        bodyCorner.x + (bodyLength * bodyScale.x) / 2, 
        bodyCorner.y + (bodyLength * bodyScale.y), 
        0);

    beakCorner = glm::vec3(headCenter.x, headCenter.y + (headRadius * headScale.y), 0);
    beakCenter = object_2D::TriangleCenter(beakCorner, beakLength, beakScale);

    eyeCenter = glm::vec3(
        headCenter.x + (headRadius * headScale.x) / 3, 
        headCenter.y + (headRadius * headScale.y) / 3, 
        0);

    lineCorner = headCenter;
    lineCenter = object_2D::SquareCenter(lineCorner, lineLength, lineScale);

    /* Rata va fi incadrata intr-un dreptunghi de latime latimea maxima a ratei si
    inaltime 1.5 * inaltimea ratei pentru a incapea in intregime in acesta, avand 
    centrul in centrul corpului ratei */
    duckHeight *= 1.5f;
    boxCorner = glm::vec3(bodyCenter.x - duckWidth / 2, bodyCenter.y - duckHeight / 2, 0);
}


// Calculul noului unghi dupa lovirea de un perete pentru a obtine directia corespunzatoare
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
            dirAngle = (2 * M_PI) - (dirAngle - M_PI);
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


// Rotatie fata de un punct de referinta
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

    // Daca rata a fost impuscata, in locul ochiului va avea un "x"
    if (currState == "shot") {
        RenderCross();
    } else {
        RenderEye();
    }

    RenderBeak();
    RenderHead();
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

    modelMatrix *= parentMatrix; // miscarea odata cu restul ratei

    // Rotatie in jurul centrului corpului, pentru rotatia in functie de directie
    RotateInRefToPoint(eyeCenter, bodyCenter, (-M_PI / 2 + dirAngle));
    
    modelMatrix *= transform_2D::Scale(eyeScale.x, eyeScale.y);

    RenderMesh2D(meshes["pupil"], shaders["VertexColor"], modelMatrix);
    RenderMesh2D(meshes["eye"], shaders["VertexColor"], modelMatrix);
}


void Tema1::RenderLine(int sense) 
{
    modelMatrix = glm::mat3(1);
    modelMatrix *= transform_2D::Translate(lineCorner.x, lineCorner.y);

    modelMatrix *= parentMatrix; // miscarea odata cu restul ratei

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

    modelMatrix *= parentMatrix; // miscarea odata cu restul ratei

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

    modelMatrix *= parentMatrix; // miscarea odata cu restul ratei

    // Rotatie in jurul centrului corpului, pentru rotatia in functie de directie
    RotateInRefToPoint(headCenter, bodyCenter, (-M_PI / 2 + dirAngle));

    modelMatrix *= transform_2D::Scale(headScale.x, headScale.y);

    RenderMesh2D(meshes["head"], shaders["VertexColor"], modelMatrix);
}


void Tema1::RenderBody() 
{
    modelMatrix = glm::mat3(1);
    modelMatrix *= transform_2D::Translate(bodyCorner.x, bodyCorner.y);

    modelMatrix *= parentMatrix; // miscarea odata cu restul ratei

    // Rotatie in jurul centrului corpului, pentru rotatia in functie de directie
    RotateInRefToPoint(bodyCorner, bodyCenter, (-M_PI / 2 + dirAngle));

    modelMatrix *= transform_2D::Scale(bodyScale.x, bodyScale.y);

    RenderMesh2D(meshes["body"], shaders["VertexColor"], modelMatrix);
}


void Tema1::RenderWing(int sense) 
{
    modelMatrix = glm::mat3(1);
    modelMatrix *= transform_2D::Translate(wingCorner.x, wingCorner.y);

    modelMatrix *= parentMatrix; // miscarea odata cu restul ratei

    // Rotatie in jurul centrului corpului, pentru rotatia in functie de directie
    RotateInRefToPoint(wingCorner, bodyCenter, (-M_PI / 2 + dirAngle));

    wingCenter = object_2D::TriangleCenter(wingCorner, wingLength, wingScale);

    // Rotatia aripii pentru pozitia initiala
    RotateInRefToPoint(wingCorner, wingCenter, (sense * (M_PI / 2)));

    // Rotatia aripii in jurul centrului sau, in sensul corespunzator
    RotateInRefToPoint(wingCorner, wingCenter, (sense * wingRotationAngle));

    glm::mat3 modelMatrixBrown = modelMatrix * transform_2D::Scale(wingScale.x, wingScale.y * 0.85f);
    glm::mat3 modelMatrixBlue = modelMatrix * transform_2D::Scale(wingScale.x, wingScale.y);

    RenderMesh2D(meshes["wingBrown"], shaders["VertexColor"], modelMatrixBrown);
    RenderMesh2D(meshes["wingBlue"], shaders["VertexColor"], modelMatrixBlue);
}


void Tema1::RenderGrass() 
{
    modelMatrix = glm::mat3(1);
    modelMatrix *= transform_2D::Scale(grassScale.x, grassScale.y);

    RenderMesh2D(meshes["grass"], shaders["VertexColor"], modelMatrix);
}


void Tema1::RenderLives() 
{
    float x = (lifeRadius * lifeScale.x) * 2;
    float y = resolution.y - (lifeRadius * lifeScale.y) * 2;

    for (int i = 0; i < lives; i++) {
        modelMatrix = glm::mat3(1);
        modelMatrix *= transform_2D::Translate(x, y);
        modelMatrix *= transform_2D::Scale(lifeScale.x, lifeScale.y);
        RenderMesh2D(meshes["life"], shaders["VertexColor"], modelMatrix);
        x += ((lifeRadius * lifeScale.x) * 2) + (lifeRadius * lifeScale.x);
    }
}


void Tema1::RenderBullets() 
{
    float x = (lifeRadius * lifeScale.x) * 2 - (bulletLength * bulletScale.x) / 2;
    float y = resolution.y - ((lifeRadius * lifeScale.y) * 2) * 2 - bulletLength * bulletScale.y;

    for (int i = 0; i < bullets; i++) {
        modelMatrix = glm::mat3(1);
        modelMatrix *= transform_2D::Translate(x, y);
        modelMatrix *= transform_2D::Scale(bulletScale.x, bulletScale.y);
        RenderMesh2D(meshes["bullet"], shaders["VertexColor"], modelMatrix);
        x += ((lifeRadius * lifeScale.x) * 2) + (lifeRadius * lifeScale.x);
    }
}

void Tema1::RenderScore() 
{
    if (score > maxScore) {
        score = maxScore;
    }

    float x = resolution.x - maxScore * scoreScale.x - 15;
    float y = resolution.y - maxScore * scoreScale.y - 15;

    modelMatrix = glm::mat3(1);
    modelMatrix *= transform_2D::Translate(x, y);
    modelMatrix *= transform_2D::Scale(scoreScale.x, scoreScale.y);
    RenderMesh2D(meshes["scoreBox"], shaders["VertexColor"], modelMatrix);

    modelMatrix = glm::mat3(1);
    modelMatrix *= transform_2D::Translate(x, y);
    modelMatrix *= transform_2D::Scale((score / maxScore) * scoreScale.x, scoreScale.y);
    RenderMesh2D(meshes["currScore"], shaders["VertexColor"], modelMatrix);
}


// Verifica daca coordonatele mouse-ului se afla in patratul ce incadreaza rata
bool Tema1::IsInBox(int mouseX, int mouseY)
{
    int x1 = tx + boxCorner.x;
    int x2 = x1 + duckWidth;

    int y1 = ty + boxCorner.y;
    int y2 = y1 + duckHeight;

    return ((mouseX >= x1 && mouseX <= x2) && (mouseY >= y1 && mouseY <= y2));
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
    if (IS_BIT_SET(button, GLFW_MOUSE_BUTTON_LEFT) && currState == "active") {
        bullets--;

        glm::ivec2 currResolution = window->GetResolution();

        /* Se modifica coordonata y a mouse-ului astfel incat sistemele
        de coordonate sa corespunda */
        mouseY = currResolution.y - mouseY;

        /* In cazul modificarii rezolutiei trebuie scalate coordonatele
        mouse-ului pentru a avea aceste coordonate in acelasi spatiu de valori */
        mouseX *= (float) resolution.x / currResolution.x;
        mouseY *= (float) resolution.y / currResolution.y;

        if (IsInBox(mouseX, mouseY)) {
            bulletsShot++;

            if ((bossFight && (bulletsShot == 3)) || (!bossFight)) {
                currState = "shot";
                score += scorePerDuck;
            }
        }
    }
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
