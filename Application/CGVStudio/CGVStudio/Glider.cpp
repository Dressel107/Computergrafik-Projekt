#include "Glider.h"
#define _USE_MATH_DEFINES
#include <math.h>



Glider::Glider(Vector spawnPosition)
{
    this->spawnPosition = spawnPosition;
    this->flyPower = 5;
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
    Matrix TM, RM;
    RM.rotationY(M_PI / 2);
    this->Transform = TM.translation(this->spawnPosition) * RM;
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

void Glider::update(float dtime, Camera& cam)
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
    std::cout << this->rotUpDown << this->rotLeftRight << std::endl;

    camMat.translation(spawnPosition + Vector(-10, 12, 0) );
    tmp =  camMat;

    cam.setPosition(tmp.translation());
    cam.setTarget(this->transform().translation() + Vector(0.1, 0.1, 0.1));

    glider->transform(this->Transform);
}

Vector Glider::update(float dtime)
{
    Matrix moveForwardMat, rotUpDownMat, rotLeftRightMat, camMat, tmp;

    //Kontinuierlich Geradeaus
    this->movingVec = Transform.forward() * flyPower;
    moveForwardMat.translation(movingVec * dtime);

    //Neigen Oben/Unten
    rotUpDownMat.rotationX(this->rotUpDown * dtime);

    //Kamera positionieren
    //cam.setPosition(Vector(this->Transform.m03+3, this->transform().m13 + 7, this->transform().m23));
    //cam.setTarget(Vector(this->Transform.m03-4, this->transform().m13+0.1, this->transform().m23+0.1)); //+0.1 muss eigentlich noch weg
    
    //Neigen Links/Rechts
    rotLeftRightMat.rotationZ(this->rotLeftRight * dtime);

    this->Transform = moveForwardMat * this->Transform * rotUpDownMat * rotLeftRightMat;

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


