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

float velocity = 1;
float lift = 0.000111788491;
float drag = 0.0000324;
float weight = 0.317522032;
float pitch = 1;


float tmpPhi = 1;

Vector unitVecNegY = Vector(0, -1, 0);
Vector unitVecPosZ = Vector(0, 0, 1);

//https://www.youtube.com/watch?v=uIgEwJVWVpY&t=368s&ab_channel=ChristopherScottVaughen
void Glider::calcNextMovment() {



    //phi = Winkel zwischen horizontalen Achse und Forwardachse des Gleiters
    float phi = acos(Transform.forward().normalize().dot(unitVecPosZ));
    if (Transform.forward().normalize().Y < 0) 
    {
        phi *= -1;
    }

    //velocity = 

    ////std::cout << phi << std::endl;

    ////tmpPhi wird für die Beschleuning benöting,so dass die Geschwindigkeits Zunahme/Abnahme nicht linear verläuft 
    //if (phi > 0)
    //{
    //    tmpPhi = tmpPhi * 1.00001;
    //    tmpPhi = std::min(tmpPhi, 2.0f);
    //}
    //else {
    //    //tmpPhi = tmpPhi * 0.9;
    //    //tmpPhi = std::max(tmpPhi, 0.1f);
    //}
    //
    //
    //velocity = velocity - (phi * tmpPhi);
    //if (velocity < -3) {
    //    velocity = -3;
    //}



    this->nextPos = Transform.forward() * velocity + Transform.backward() * drag + Transform.up() * lift + unitVecNegY * weight;
    this->nextRot = this->rotUpDown * pitch;
    
}


