#include "Glider.h"
#define _USE_MATH_DEFINES
#include <math.h>


Vector unitVecNegY = Vector(0, -1, 0);
Vector unitVecPosZ = Vector(0, 0, 1);
Vector unitVecPosX = Vector(1, 0, 0);

Glider::Glider(Vector spawnPosition)
{
    this->spawnPosition = spawnPosition;
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
    this->glider->BoundingBox.translate(spawnPosition);
    return true;
}

/// <summary>
///     Wird aufgerufen, wenn der Gleiter mit einem festen Gegenstand, bspw. dem Terrain kollidiert.
/// </summary>
void Glider::crash()
{
    this->isCrashed = true;
}

/// <summary>
///     Wird aufgerufen, wenn sich der Gleiter in einem Aufwind befindet.
/// </summary>
void Glider::upwind(float dtime, Vector windPos)
{
    float phi = acos(Transform.forward().normalize().dot(unitVecPosZ));


    float distance = (windPos - this->transform().translation()).length();
    float lift = 1000 / (distance * distance);


    Matrix MT, RT;
    Vector nextPos = Transform.up() * lift;
    float nextRot = -((10 - (phi*phi)) / distance);
    MT.translation(nextPos * dtime);
    RT.rotationX(nextRot * dtime);
    this->Transform = MT * this->Transform * RT;

    std::cout << lift << std::endl;
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
    // Wenn Gleiter gecrasht ist, auf Startposition zurücksetzen
    if (this->isCrashed)
    {
        Matrix moveToSpawnPos;
        moveToSpawnPos.translation(this->spawnPosition);

        Transform = moveToSpawnPos;
        this->isCrashed = false;
    }
    else
    {
        if (this->start == false) 
        {
            return this->Transform.translation();
        }
        Matrix moveForwardMat, rotUpDownMat, rotLeftRightMat;
        calcNextMovment();

        //Kontinuierlich Geradeaus
        Vector gliderMovement = this->nextPos * dtime;
        moveForwardMat.translation(gliderMovement);

        //Neigen Oben/Unten
        rotUpDownMat.rotationX(this->nextRotX * dtime);

        //Neigen Links/Rechts
        rotLeftRightMat.rotationZ(this->nextRotZ * dtime);

        this->Transform = moveForwardMat * this->Transform * rotUpDownMat * rotLeftRightMat;
    }

    // Gleiter bewegen
    glider->transform(this->Transform);

    // BoundingBox aktualisieren
    //this->glider->BoundingBox.translate(this->Transform.translation());
    this->glider->BoundingBox.transform(this->Transform);

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




float velocity = 300;
float lift = 0.000111788491;
float drag = 0.0000324;
float weight = 0.9317522032;

float pitch = 0;

//https://www.youtube.com/watch?v=uIgEwJVWVpY&t=368s&ab_channel=ChristopherScottVaughen
void Glider::calcNextMovment() {

    float phi = acos(Transform.forward().normalize().dot(unitVecPosZ));
    if (Transform.forward().normalize().Y < 0) 
    {
        phi *= -1;
    }

    float omega = acos(Transform.right().normalize().dot(unitVecPosX));
    if (Transform.right().normalize().Y < 0)
    {
        omega *= -1;
    }
    if (omega < 00.1 && omega > -00.1) {
        omega = 0;
    }

    velocity = velocity - phi * std::abs(phi) / 50;
    if (velocity < -1) {
        velocity = -1;
    }
    if (velocity > 6) {
        velocity = 6;
    }

    if (phi < -M_PI / 2 && phi > M_PI / 2) {
        pitch = 0;
    }
    else {
        pitch = 1 / ((abs(velocity)+1) * 10);
    }


    //std::cout << velocity << "|" << pitch <<  "|" << phi << "|" << omega << std::endl;

    this->nextPos =  Transform.forward() * 2.5 * velocity  + Transform.backward() * drag + Transform.up() * lift +  unitVecNegY * weight;
    this->nextRotX =  (this->rotUpDown / 5) + pitch;
    this->nextRotZ = this->rotLeftRight - ((omega * abs(omega))  / 4);
    
}

void Glider::reset() {
    this->start = false;
    //Gleiter an Starposition setzen
    Matrix TM;
    this->Transform = TM.translation(this->spawnPosition);
    glider->transform(this->Transform);
    velocity = 1;
    pitch = 0;
    this->glider->BoundingBox.translate(spawnPosition);
}


