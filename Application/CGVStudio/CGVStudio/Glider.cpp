#include "Glider.h"
#define _USE_MATH_DEFINES
#include <math.h>



Glider::Glider(Vector spawnPosition, Camera* cam)
{
    this->spawnPosition = spawnPosition;
    this->flyPower = 3;
    this->rotLeftRight = 0;
    this->rotUpDown = 0;
    this->camTransform.identity();
    this->cam = cam;
}
Glider::~Glider()
{
    delete glider;
}

bool Glider::loadModel(const char* gliderFile)
{
    glider = new Model(gliderFile, false);
    glider->shader(shader());


    //Gleiter an Starposition setzen
    Matrix TM, RM;
    RM.rotationY(M_PI / 2);
    this->Transform = TM.translation(this->spawnPosition);
    glider->transform(this->Transform);

    //cam inital positionieren
    Matrix camTM;
    this->camTransform = camTM.translation(this->spawnPosition);
    this->cam->setPosition(camTM.translation());
    this->cam->setTarget(this->transform().translation() + Vector(0.1, 0.1, 0.1));


    return true;
}

/// <summary>
///     Wird aufgerufen, wenn der Gleiter mit einem festen Gegenstand, bspw. dem Terrain kollidiert.
/// </summary>
void Glider::crash()
{

}

/// <summary>
///     Wird aufgerufen, wenn sich der Gleiter in einem Aufwind befindet.
/// </summary>
void Glider::upwind()
{

}

/// <summary>
///     Navigiert den Gleiter in eine bestimmte Richtung.
/// </summary>
/// <param name="UpDown">1.0f: Up, -1.0f: Down, 0.0f: Geradeaus</param>
/// <param name="LeftRight">1.0f: Right, -1.0f: Left, 0.0f: Geradeaus</param>
void Glider::navigate(float UpDown, float LeftRight)
{
    this->rotUpDown = UpDown;
    this->rotLeftRight = LeftRight;
}

Vector Glider::update(float dtime)
{
    Matrix moveForwardMat, rotUpDownMat, rotLeftRightMat, camMat, tmp;

    //Kontinuierlich Geradeaus
    this->movingVec = Transform.forward() * flyPower;
    moveForwardMat.translation(movingVec * dtime);

    //Neigen Oben/Unten
    rotUpDownMat.rotationX(this->rotUpDown * dtime);

    //Neigen Links/Rechts
    rotLeftRightMat.rotationZ(this->rotLeftRight * dtime);
    this->Transform =  moveForwardMat * this->Transform * rotUpDownMat * rotLeftRightMat;

    glider->transform(this->Transform);
    return this->Transform.translation();
}



void Glider::navigateForTesting(float forwardBackward, float UpDown, float LeftRight)
{
    //this->movingVector = Transform.right() * forwardBackward;

}

void Glider::draw(const BaseCamera& Cam)
{
    glider->draw(Cam);
}


