//
//  Application.hpp
//  ogl4
//
//  Created by Philipp Lensing on 16.09.16.
//  Copyright © 2016 Philipp Lensing. All rights reserved.
//

#ifndef Application_hpp
#define Application_hpp

#include <stdio.h>
#include <list>
#include "camera.h"
#include "phongshader.h"
#include "constantshader.h"
#include "vertexbuffer.h"
#include "indexbuffer.h"
#include "basemodel.h"
#include "terrain.h"
#include "../CGVStudio/CGVStudio/Glider.h"
#include "../CGVStudio/CGVStudio/Capsule.h"
#include "../CGVStudio/CGVStudio/IAnimatable.h"
#include "../CGVStudio/CGVStudio/SpeedDisc.h"
#include "../CGVStudio/CGVStudio/Sphere.h"
#include "../CGVStudio/CGVStudio/Wind.h"
#include "../CGVStudio/CGVStudio/WindTurbine.h"
#include "../CGVStudio/CGVStudio/Decoration.h"

class Application
{
public:
    typedef std::list<BaseModel*> ModelList;
    typedef std::list<Sphere*> SphereList;
    typedef std::list<Capsule*> CapsuleList;
    typedef std::list<SpeedDisc*> SpeedDiscList;
    typedef std::list<Wind*> WindList;
    typedef std::list<WindTurbine*> WindTurbineList;
    typedef std::list<Decoration*> DecorationList;
    Application(GLFWwindow* pWin);
    void start();
    void update(float dtime);
    void draw();
    void end();

protected:
    Camera Cam;
    ModelList Models;
    GLFWwindow* pWindow;
    Glider* glider;
    Terrain* pTerrain;
    
    SphereList Spheres;
    CapsuleList Capsules;
    SpeedDiscList SpeedDiscs;
    WindList Winds;
    WindTurbineList WindTurbines;
    DecorationList Decorations;
    Matrix camTM;
    Matrix targetTM;

    void spawnDynamicObjects();
    void handleObjectCollisions();
    void handleUpwindsCollisions(float dtime);
    void handleTerrainCollision();
    void updateObjects(float dtime);
    void updateUITexts();
    void gameOver();
    void restartGame();
    Vector getRandomSpawnPosition();
    Vector getRandomSpawnPositionOnTerrain();
    void drawText(const char* text, float x, float y);
    void lockCamToModel(Camera& cam, BaseModel* model);
};

#endif /* Application_hpp */
