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
#include <GL/freeglut.h>
#include <imgui.h>

#ifdef WIN32
#define ASSET_DIRECTORY "../../assets/"
#else
#define ASSET_DIRECTORY "../assets/"
#endif

double oldMousePosX = 0;
double oldMousePosY = 0;
const int SPHERES_COUNT = 60;
const int CAPSULES_COUNT = 20;
const int UPWINDS_COUNT = 10;
const int SPEED_DISCS_30_COUNT = 2;
const int SPEED_DISCS_50_COUNT = 6;
const int SPEED_DISCS_75_COUNT = 3;
const int SPEED_DISCS_90_COUNT = 2;
const int SPEED_DISCS_110_COUNT = 2;
const int WIND_TURBINES_COUNT = 40;
const int BUSHES_COUNT = 60;
const int WOODS_COUNT = 50;
const int MAX_SPAWN_Y = 12;
const int TERRAIN_SCALE = 10;
Vector playerSpawnPosition(2, 38, -30);

//Normale sicht
Vector cameraPositionRelativToModel(0, 10, -10);
Vector cameraTargetRelativToModel(0, 0, 5);


int points = 0;
int height = 0;
bool isGameOver = false;
int startTime = 0;

Application::Application(GLFWwindow* pWin) : pWindow(pWin), Cam(pWin)
{
    BaseModel* pModel;
    PhongShader* pPhongShader;
    
    // create LineGrid model with constant color shader
 //   pModel = new LinePlaneModel(10, 10, 10, 10);
 //   ConstantShader* pConstShader = new ConstantShader();
	//pConstShader->color( Color(1,0,0));
 //   pModel->shader(pConstShader, true);
 //   // add to render list
 //   Models.push_back( pModel );
    
    // Skybox laden    
    pModel = new Model(ASSET_DIRECTORY "skybox.obj", false);
    pModel->shader(new PhongShader(), true);
    Models.push_back(pModel);

    // Terrain laden
    pTerrain = new Terrain();
    TerrainShader* pTerrainShader = new TerrainShader(ASSET_DIRECTORY);
    pTerrainShader->diffuseTexture(Texture::LoadShared(ASSET_DIRECTORY "grass.bmp"));
    pTerrain->shader(pTerrainShader, true);
    pTerrain->load(ASSET_DIRECTORY "heightmap2.png", ASSET_DIRECTORY"grass.bmp", ASSET_DIRECTORY"rock.bmp");
    pTerrain->width(TERRAIN_SCALE);
    pTerrain->depth(TERRAIN_SCALE);
    pTerrain->height(TERRAIN_SCALE);
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
    //spawnDynamicObjects();

    //drawText("Geschwindigkeit", 10, 10);
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

    //ImGui::Begin("My Window");
    //ImGui::Text("This is some text");
    //ImGui::End();

    // Wenn GameOver ist, warten bis der Benutzer die Leertaste drückt
    if (isGameOver)
    {
        // TODO: Text anzeigen

        if (glfwGetKey(pWindow, GLFW_KEY_SPACE) == GLFW_PRESS)
        {
            restartGame();            
        }
    }

    // Tastaturdaten zur Navigation des Gleiters einlesen
    float forwardBackward = 0.0f;
    float upDown = 0.0f;
    float leftRight = 0.0f;

    // Nach vorne/hinten bewegen
    if (glfwGetKey(pWindow, GLFW_KEY_V) == GLFW_PRESS)
    {
        forwardBackward = 1.0f;
    }
    if (glfwGetKey(pWindow, GLFW_KEY_B) == GLFW_PRESS)
    {
        forwardBackward = -1.0f;
    }
    
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
        this->camTM.translation(Vector(0, 1, 3));
        this->targetTM.translation(Vector(0, 1, 3));
    }

    // Gleiter navigieren
    //glider->navigateForTesting(forwardBackward, upDown, leftRight);
    glider->navigate(upDown, leftRight);

    // Objekte aktualisieren
    glider->update(dtime);
    lockCamToModel(Cam, glider);

    //updateObjects(dtime);


    // Kollisionen prüfen
    //handleCollectablesCollisions();
    //handleUpwindsCollisions();
    //handleTerrainCollision();
    //Cam.setUp(Vector(0, 0, 1));
    Cam.update();

    // Prüfen, ob Zeit abgelaufen ist
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
        Vector spawnPosition = getRandomSpawnPosition();

        pPhongShader = new PhongShader();
        Wind* wind = new Wind(spawnPosition);
        wind->shader(pPhongShader, true);
        wind->loadModel(ASSET_DIRECTORY "upwind.dae");
        Models.push_back(wind);
        Winds.push_back(wind);
    }

    for (int i = 0; i < SPHERES_COUNT; i++)
    {
        Vector spawnPosition = getRandomSpawnPosition();

        pPhongShader = new PhongShader();
        Sphere* sphere = new Sphere(spawnPosition);
        sphere->shader(pPhongShader, true);
        sphere->loadModel(ASSET_DIRECTORY "sphere.dae");
        Models.push_back(sphere);
        Spheres.push_back(sphere);
    }

    for (int i = 0; i < CAPSULES_COUNT; i++)
    {
        Vector spawnPosition = getRandomSpawnPosition();

        pPhongShader = new PhongShader();
        Capsule* capsule = new Capsule(spawnPosition);
        capsule->shader(pPhongShader, true);
        capsule->loadModel(ASSET_DIRECTORY "capsule.dae");
        Models.push_back(capsule);
        Capsules.push_back(capsule);
    }

    for (int i = 0; i < SPEED_DISCS_30_COUNT; i++)
    {
        Vector spawnPosition = getRandomSpawnPosition();

        pPhongShader = new PhongShader();
        SpeedDisc* disc = new SpeedDisc(spawnPosition);
        disc->shader(pPhongShader, true);
        disc->loadModel(SpeedDisc::Min30);
        Models.push_back(disc);
        SpeedDiscs.push_back(disc);
    }

    for (int i = 0; i < SPEED_DISCS_50_COUNT; i++)
    {

    }

    for (int i = 0; i < SPEED_DISCS_75_COUNT; i++)
    {

    }

    for (int i = 0; i < SPEED_DISCS_90_COUNT; i++)
    {

    }

    for (int i = 0; i < SPEED_DISCS_110_COUNT; i++)
    {

    }
}

void Application::handleTerrainCollision()
{
    for (int x = 1; x < pTerrain->imgWidth; x+=4) 
    {
        for (int y = 1; y < pTerrain->imgHeight; y+=4)
        {
            Vector v = pTerrain->tmpVertices[(y) * pTerrain->imgWidth + (x)];
            AABB bb = glider->boundingBox();

            // Wenn sich die BoundingBox des Gleiters mit einem Vertex schneidet, hat der Spieler das Terrain berührt
            if (bb.intersectWith(v))
            {
                glider->crash();
                gameOver();
            }
        }
    }

    // Version 2

}

/// <summary>
///     Prüft, ob der Spieler ein einsammelbares Objekt berührt.
/// </summary>
void Application::handleCollectablesCollisions()
{
    AABB bb = this->glider->boundingBox();

    for each (Capsule* capsule in Capsules)
    {
        // Prüfen, ob Spieler das Collectable berührt
        if (bb.intersectWith(capsule->boundingBox()))
        {
            points += capsule->collect();
            Capsules.remove(capsule);
            Models.remove(capsule);
        }
    }

    for each (Sphere* sphere in Spheres)
    {
        // Prüfen, ob Spieler das Collectable berührt
        if (bb.intersectWith(sphere->boundingBox()))
        {
            points += sphere->collect();
            Spheres.remove(sphere);
            Models.remove(sphere);
        }
    }

    //for each (SpeedDisc* speedDisc in SpeedDiscs)
    //{
    //    // Prüfen, ob Spieler das Collectable berührt
    //    if (bb.intersectWith(speedDisc->boundingBox()))
    //    {
    //        points += speedDisc->collect();
    //        SpeedDiscs.remove(speedDisc);
    //        Models.remove(speedDisc);
    //    }
    //}
}

/// <summary>
///     Prüft, ob sich der Spieler in einem Aufwind befindet.
/// </summary>
void Application::handleUpwindsCollisions()
{
    AABB bb = this->glider->boundingBox();

    for each (Wind* wind in Winds)
    {
        if (bb.intersectWith(wind->boundingBox()))
        {
            this->glider->upwind();
        }
    }
}

/// <summary>
///     Aktualisiert die Positionen aller Objekte in der Szene.
/// </summary>
void Application::updateObjects(float dtime)
{

    for each (Sphere* sphere in Spheres)
    {
        sphere->update(dtime);
    }

    for each (Capsule* capsule in Capsules)
    {
        capsule->update(dtime);
    }

    for each (SpeedDisc* disc in SpeedDiscs)
    {
        disc->update(dtime);
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
}

/// <summary>
///     Aktualisiert alle Texte auf der UI.
/// </summary>
void Application::updateUITexts()
{

}

void Application::gameOver()
{
    isGameOver = true;
    points = 0;
}

void Application::restartGame()
{
    isGameOver = false;
}

/// <summary>
///     Gibt eine zuällige Position über dem Terrain für ein Objekt zurück.
/// </summary>
Vector Application::getRandomSpawnPosition()
{
    int indexX = rand() % pTerrain->imgWidth;
    int indexZ = rand() % pTerrain->imgHeight;
    Vector vertex = pTerrain->tmpVertices[(indexZ)*pTerrain->imgWidth + (indexX)];
    float y = vertex.Y + (rand() % MAX_SPAWN_Y);

    return Vector(vertex.X * TERRAIN_SCALE, y, vertex.Z * TERRAIN_SCALE);
}

Vector Application::getRandomSpawnPositionOnTerrain()
{
    int indexX = rand() % pTerrain->imgWidth;
    int indexZ = rand() % pTerrain->imgHeight;
    Vector vertex = pTerrain->tmpVertices[(indexZ)*pTerrain->imgWidth + (indexX)];

    return Vector(vertex.X * TERRAIN_SCALE, (vertex.Y * TERRAIN_SCALE) - 0.1, vertex.Z * TERRAIN_SCALE);
}

float Application::getRandomValue(float min, float max)
{
    if (min < 0)
    {
        min = -min;
    }

    float scale = rand() / (float)RAND_MAX; /* [0, 1.0] */
    return -min + scale * (max - -min);      /* [min, max] */
}

void Application::drawText(const char* text, float x, float y) 
{
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(255.0, 0.0, 0.0);
    glRasterPos2i(x, y);
    glutBitmapString(GLUT_BITMAP_9_BY_15, (const unsigned char*)text);
    //glFlush();
    glutSwapBuffers();

    //glRasterPos2f(x, y);

    //for (int i = 0; text[i] != '\0'; i++) 
    //{
    //    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, text[i]);
    //}
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