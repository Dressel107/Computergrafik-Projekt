//
//  Application.cpp
//  ogl4
//
//  Created by Philipp Lensing on 16.09.16.
//  Copyright © 2016 Philipp Lensing. All rights reserved.
//

#include "Application.h"
#ifdef WIN32
#include <GL/glew.h>
#include <glfw/glfw3.h>
#define _USE_MATH_DEFINES
#include <math.h>
#else
#define GLFW_INCLUDE_GLCOREARB
#define GLFW_INCLUDE_GLEXT
#include <glfw/glfw3.h>
#endif
#include "lineplanemodel.h"
#include "triangleplanemodel.h"
#include "trianglespheremodel.h"
#include "lineboxmodel.h"
#include "triangleboxmodel.h"
#include "model.h"
#include "terrainshader.h"
#include "../CGVStudio/CGVStudio/Upwind.h"
#include <imgui.h>

#ifdef WIN32
#define ASSET_DIRECTORY "../../assets/"
#else
#define ASSET_DIRECTORY "../assets/"
#endif

const int SPHERES_COUNT = 50;
const int CAPSULES_COUNT = 25;
const int UPWINDS_COUNT = 12;
const int WIND_TURBINES_COUNT = 25;
const int BUSHES_COUNT = 30;
const int WOODS_COUNT = 30;

const int MIN_UPWIND_SPAWN_Y = 0;
const int MAX_UPWIND_SPAWN_Y = 15;
const int MIN_COLLECTABLE_SPAWN_Y = 2;
const int MAX_COLLECTABLE_SPAWN_Y = 50;
const int TERRAIN_SCALING = 40;

Vector playerSpawnPosition(0, 100, -120);

//Normale sicht
Vector cameraPositionRelativToModel(0, 7, -10);
Vector cameraTargetRelativToModel(0, 0, 5);

int points = 0;
int height = 0;
bool isGameOver = false;
int startTime = 0;

BaseModel* pSkybox;

Application::Application(GLFWwindow* pWin) : pWindow(pWin), Cam(pWin)
{
    BaseModel* pModel;
    PhongShader* pPhongShader;
   
    // Skybox laden    
    pPhongShader = new PhongShader();
    pModel = new Model(ASSET_DIRECTORY "skybox.obj", false);
    pModel->shader(pPhongShader, true);
    Models.push_back(pModel);
    pSkybox = pModel;

    // Terrain laden
    pTerrain = new Terrain();
    TerrainShader* pTerrainShader = new TerrainShader(ASSET_DIRECTORY);
    pTerrainShader->diffuseTexture(Texture::LoadShared(ASSET_DIRECTORY "grass.bmp"));
    pTerrain->shader(pTerrainShader, true);
    pTerrain->load(ASSET_DIRECTORY "h.png", ASSET_DIRECTORY"grass.bmp", ASSET_DIRECTORY"rock.bmp", TERRAIN_SCALING);
    pTerrain->width(TERRAIN_SCALING);
    pTerrain->depth(TERRAIN_SCALING);
    pTerrain->height(TERRAIN_SCALING);
    pTerrain->setK(100);
    Models.push_back(pTerrain);
        
    // Spieler (Gleiter) laden
    pPhongShader = new PhongShader();
    glider = new Glider(playerSpawnPosition);
    glider->shader(pPhongShader, true);
    glider->loadModel(ASSET_DIRECTORY "glider.dae");
    Models.push_back(glider);

    //Kameraeinstellungen
    this->camTM.translation(cameraPositionRelativToModel);
    this->targetTM.translation(cameraTargetRelativToModel);

    //Dynamische Objekte spawnen
    spawnDynamicObjects();

}
void Application::start()
{
    glEnable (GL_DEPTH_TEST); // enable depth-testing
    glDepthFunc (GL_LESS); // depth-testing interprets a smaller value as "closer"
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);    
}

void Application::update(float dtime)
{
    // Gleiter starten
    if (glfwGetKey(pWindow, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        this->glider->startGlider();
    }

    // Spiel restarten
    if (glfwGetKey(pWindow, GLFW_KEY_R) == GLFW_PRESS)
    {
        this->restartGame();
    }

    // Tastaturdaten zur Navigation des Gleiters einlesen
    float upDown = 0.0f;
    float leftRight = 0.0f;
    
    // Nach oben/unten fliegen
    if (glfwGetKey(pWindow, GLFW_KEY_UP) == GLFW_PRESS)
    {
        upDown = -1.0f;
    }
    if (glfwGetKey(pWindow, GLFW_KEY_DOWN) == GLFW_PRESS) 
    {
        upDown = 1.0f;
    }

    // Nach links/rechts fliegen
    if (glfwGetKey(pWindow, GLFW_KEY_LEFT) == GLFW_PRESS) 
    {
        leftRight = -1.0f;
    }
    if (glfwGetKey(pWindow, GLFW_KEY_RIGHT) == GLFW_PRESS) 
    {
        leftRight = 1.0f;
    }

    //Sicht ändern
    if (glfwGetKey(pWindow, GLFW_KEY_1) == GLFW_PRESS)//normale Sicht
    {
        this->camTM.translation(cameraPositionRelativToModel);
        this->targetTM.translation(cameraTargetRelativToModel);
    }

    if (glfwGetKey(pWindow, GLFW_KEY_2) == GLFW_PRESS)//Rückwärts sicht
    {
        this->camTM.translation(Vector(0,10,10));
        this->targetTM.translation(Vector(0,0,-5));
    }

    if (glfwGetKey(pWindow, GLFW_KEY_3) == GLFW_PRESS)// Cockpitsicht
    {
        this->camTM.translation(Vector(0, 1.4, 3.2)); 
        this->targetTM.translation(Vector(0, 1.4, 4.3));
    }

    // Gleiter navigieren
    glider->navigate(upDown, leftRight);

    // Objekte aktualisieren
    lockCamToModel(Cam, glider);
    updateObjects(dtime);

    // Kollisionen prüfen
    handleObjectCollisions();
    handleTerrainCollision();
    
    Cam.update();
}

void Application::spawnDynamicObjects()
{
    srand(time(NULL));
    PhongShader* pPhongShader;

    for (int i = 0; i < WOODS_COUNT; i++)
    {
        Vector spawnPosition = getRandomSpawnPositionOnTerrain();

        pPhongShader = new PhongShader();
        Decoration* wood = new Decoration(spawnPosition);
        wood->shader(pPhongShader, true);
        wood->loadModel(ASSET_DIRECTORY "wood.dae");
        Models.push_back(wood);
        Decorations.push_back(wood);
    }

    for (int i = 0; i < BUSHES_COUNT; i++)
    {
        Vector spawnPosition = getRandomSpawnPositionOnTerrain();

        pPhongShader = new PhongShader();
        Decoration* bush = new Decoration(spawnPosition);
        bush->shader(pPhongShader, true);
        bush->loadModel(ASSET_DIRECTORY "bush.dae");
        Models.push_back(bush);
        Decorations.push_back(bush);
    }

    for (int i = 0; i < WIND_TURBINES_COUNT; i++)
    {
        Vector spawnPosition = getRandomSpawnPositionOnTerrain();

        pPhongShader = new PhongShader();
        WindTurbine* turbine = new WindTurbine(spawnPosition);
        turbine->shader(pPhongShader, true);
        turbine->loadModel(ASSET_DIRECTORY "wind_turbine_pole.dae", ASSET_DIRECTORY "wind_turbine_wheel.dae");
        Models.push_back(turbine);
        WindTurbines.push_back(turbine);
    }

    for (int i = 0; i < UPWINDS_COUNT; i++)
    {
        Vector spawnPosition = getRandomSpawnPosition(MIN_UPWIND_SPAWN_Y, MAX_UPWIND_SPAWN_Y);

        pPhongShader = new PhongShader();
        Wind* wind = new Wind(spawnPosition);
        wind->shader(pPhongShader, true);
        wind->loadModel(ASSET_DIRECTORY "upwind.dae");
        Models.push_back(wind);
        Winds.push_back(wind);
        wind->pShader = pPhongShader;
    }

    for (int i = 0; i < SPHERES_COUNT; i++)
    {
        Vector spawnPosition = getRandomSpawnPosition(MIN_COLLECTABLE_SPAWN_Y, MAX_COLLECTABLE_SPAWN_Y);

        pPhongShader = new PhongShader();
        Sphere* sphere = new Sphere(spawnPosition);
        sphere->shader(pPhongShader, true);
        sphere->loadModel(ASSET_DIRECTORY "sphere.dae");
        Models.push_back(sphere);
        Spheres.push_back(sphere);
    }

    for (int i = 0; i < CAPSULES_COUNT; i++)
    {
        Vector spawnPosition = getRandomSpawnPosition(MIN_COLLECTABLE_SPAWN_Y, MAX_COLLECTABLE_SPAWN_Y);

        pPhongShader = new PhongShader();
        Capsule* capsule = new Capsule(spawnPosition);
        capsule->shader(pPhongShader, true);
        capsule->loadModel(ASSET_DIRECTORY "capsule.dae");
        Models.push_back(capsule);
        Capsules.push_back(capsule);
    }
}

/// <summary>
///     Prüft, ob der Gleiter das Terrain berührt.
/// </summary>
void Application::handleTerrainCollision()
{
    bool intersect = this->pTerrain->intersectWith(this->glider->boundingBox());

    if (intersect)
    {
        this->glider->crash();
    }
}

/// <summary>
///     Prüft, ob der Spieler ein bestimmtes Objekt berührt und führt entsprechende Aktionen aus.
/// </summary>
void Application::handleObjectCollisions()
{
    AABB bb = this->glider->boundingBox();

    // Prüfen, ob der Spieler eine Kapsel einsammelt
    for each (Capsule* capsule in Capsules)
    {
        AABB capsuleBB = capsule->boundingBox();

        if (bb.intersectWith(capsule->boundingBox()))
        {
            points += capsule->collect();
            Models.remove(capsule);
        }
    }

    // Prüfen, ob der Spieler eine Kugel einsammelt
    for each (Sphere* sphere in Spheres)
    {
        if (bb.intersectWith(sphere->boundingBox()))
        {
            points += sphere->collect();
            Models.remove(sphere);
        }
    }

    // Prüfen, ob der Spieler gegen ein Windrad fliegt
    for each (WindTurbine* windTurbine in WindTurbines)
    {
        if (bb.intersectWith(windTurbine->boundingBoxPole()) || bb.intersectWith(windTurbine->boundingBoxWheel()))
        {
            this->glider->crash();
        }
    }

    // Prüfen, ob sich der Spieler in einem Aufwind befindet
    for each (Wind* wind in Winds)
    {
        if (bb.intersectWith(wind->boundingBox()))
        {
            this->glider->upwind(wind);
        }
    }
}

/// <summary>
///     Aktualisiert die Positionen aller Objekte in der Szene.
/// </summary>
float currentRotation = 0;
void Application::updateObjects(float dtime)
{
    currentRotation = currentRotation + (-2 * M_PI / 200 * dtime);
    Matrix RM, SM;
    RM.rotationY(currentRotation);
    SM.scale(5);
    pSkybox->transform(RM * SM);

    for each (Sphere* sphere in Spheres)
    {
        sphere->update(dtime);
    }

    for each (Capsule* capsule in Capsules)
    {
        capsule->update(dtime);
    }

    for each (Wind* wind in Winds)
    {
        wind->update(dtime);
    }

    for each (WindTurbine* turbine in WindTurbines)
    {
        turbine->update(dtime);
    }

    for each (Decoration* decoration in Decorations)
    {
        decoration->update(dtime);
    }
    glider->update(dtime);

}

void Application::restartGame()
{
    this->glider->reset();
    for each (Wind * wind in Winds)
    {
        wind->reset();
    }   
}

/// <summary>
///     Gibt eine zuällige Position über dem Terrain für ein Objekt zurück.
/// </summary>
Vector Application::getRandomSpawnPosition(int minY, int maxY)
{
    int indexX = rand() % pTerrain->imgWidth;
    int indexZ = rand() % pTerrain->imgHeight;
    Vector vertex = pTerrain->tmpVertices[(indexZ)*pTerrain->imgWidth + (indexX)];
    float y = (vertex.Y * TERRAIN_SCALING) + (rand() % maxY) + minY;

    return Vector(vertex.X * TERRAIN_SCALING, y, vertex.Z * TERRAIN_SCALING);
}

/// <summary>
///     Gibt eine zuällige Position auf dem Terrain für ein Objekt zurück.
/// </summary>
Vector Application::getRandomSpawnPositionOnTerrain()
{
    int indexX = rand() % pTerrain->imgWidth;
    int indexZ = rand() % pTerrain->imgHeight;
    Vector vertex = pTerrain->tmpVertices[(indexZ)*pTerrain->imgWidth + (indexX)];

    return Vector(vertex.X * TERRAIN_SCALING, (vertex.Y * TERRAIN_SCALING) - 0.1, vertex.Z * TERRAIN_SCALING);
}



void Application::draw()
{
    // 1. clear screen
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 2. setup shaders and draw models
    for( ModelList::iterator it = Models.begin(); it != Models.end(); ++it )
    {
        (*it)->draw(Cam);
    }
    
    // 3. check once per frame for opengl errors
    GLenum Error = glGetError();
    assert(Error==0);
}
void Application::end()
{
    for( ModelList::iterator it = Models.begin(); it != Models.end(); ++it )
        delete *it;
    
    Models.clear();
}

void Application::lockCamToModel(Camera& cam, BaseModel* model)
{
    //Position
    cam.setPosition((model->transform() * this->camTM).translation());

    //Up
    Vector gliederUp = model->transform().up().normalize();
    cam.setUp(this->camTM.transformVec3x3(gliederUp));

    //target
    cam.setTarget((model->transform() * this->targetTM).translation());
}

