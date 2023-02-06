#include "Glider.h"
#define _USE_MATH_DEFINES
#include <math.h>

Glider::Glider(Vector spawnPosition)
{
    this->spawnPosition = spawnPosition;
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

    RM.rotationY(M_PI / -2);
    
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
}

void Glider::update(float dtime, Camera& cam)
{
    Matrix moveForward;

    moveForward.translation(movingVector * dtime);
    this->Transform = moveForward * this->Transform;

    //Kamera positionieren
   // cam.setPosition(Vector(this->Transform.m03+3, this->transform().m13 + 7, this->transform().m23));
    cam.setTarget(Vector(this->Transform.m03-4, this->transform().m13+0.1, this->transform().m23+0.1)); //+0.1 muss eigentlich noch weg
    

    //currentPos = currentPos + (-2 * M_PI / 6 * dtime);

    //Matrix RM;
    //RM.rotationY(currentPos);
    glider->transform(this->Transform);
}

void Glider::navigateForTesting(float forwardBackward, float UpDown, float LeftRight)
{
    this->movingVector = Transform.right() * forwardBackward;

}

void Glider::draw(const BaseCamera& Cam)
{
    glider->draw(Cam);
}


