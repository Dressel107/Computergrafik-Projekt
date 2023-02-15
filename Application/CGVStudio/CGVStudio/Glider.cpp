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
void Glider::upwind()
{
    std::cout << "Aufwind";
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
        rotUpDownMat.rotationX(this->nextRot * dtime);

        //Neigen Links/Rechts
        rotLeftRightMat.rotationZ(this->rotLeftRight * dtime);

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



Vector unitVecNegY = Vector(0, -1, 0);
Vector unitVecPosZ = Vector(0, 0, 1);

float velocity = 5;
float lift = 0.000111788491;
float drag = 0.0000324;
float weight = 0.317522032;

float pitch = 0;

//https://www.youtube.com/watch?v=uIgEwJVWVpY&t=368s&ab_channel=ChristopherScottVaughen
void Glider::calcNextMovment() {
  


    float phi = acos(Transform.forward().normalize().dot(unitVecPosZ));
    phi = phi * std::abs(phi);
    if (Transform.forward().normalize().Y < 0) 
    {
        phi *= -1;
    }


    velocity = velocity - phi  / 1000;
    if (velocity < -1) {
        velocity = -1;
    }
    if (velocity > 10) {
        velocity = 10;
    }

    if (phi > -M_PI / 2 && phi < M_PI / 2) {
        pitch = 1 / velocity;
    }

    //std::cout << velocity << "|" << pitch << std::endl;



    this->nextPos = Transform.forward() * velocity  + Transform.backward() * drag + Transform.up() * lift +  unitVecNegY * weight;
    this->nextRot = this->rotUpDown + pitch;
    
}


