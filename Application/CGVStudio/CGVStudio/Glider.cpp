#include "Glider.h"
#define _USE_MATH_DEFINES
#include <math.h>



Glider::Glider(Vector spawnPosition)
{
    this->spawnPosition = spawnPosition;
    this->flyPower = 3;
    this->rotationPower = 1;
    this->rotLeftRight = 0;
    this->rotUpDown = 0;
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
    Matrix TM;
    this->Transform = TM.translation(this->spawnPosition);
    glider->transform(this->Transform);
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
    Matrix moveForwardMat, rotUpDownMat, rotLeftRightMat;
    calcNextMovment();

    //Kontinuierlich Geradeaus
    Vector gliderMovement = this->nextPos * dtime ;
    moveForwardMat.translation(gliderMovement);

    //Neigen Oben/Unten
    rotUpDownMat.rotationX(this->nextRot * dtime);

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

void Glider::calcNextMovment() {

    float lift = 0;
    float drag = 0;
    float weight = 1;
    float velocity = 0;

    float pitch = 1;

    Vector unitVecDown = Vector(0, -1,  0);

    this->nextPos = Transform.forward() * velocity + Transform.backward() * drag + Transform.up() * lift +  unitVecDown * weight;
    this->nextRot = this->rotUpDown * pitch;
    
}


