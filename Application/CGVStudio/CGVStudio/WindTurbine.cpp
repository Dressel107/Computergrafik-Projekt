#include "WindTurbine.h"
#define _USE_MATH_DEFINES
#include <math.h>

WindTurbine::WindTurbine(Vector spawnPosition)
{
    this->spawnPosition = spawnPosition;
}

WindTurbine::~WindTurbine()
{
    delete pole;
    delete wheel;
}

bool WindTurbine::loadModel(const char* poleFile, const char* wheelFile)
{
    pole = new Model(poleFile, false);
    pole->shader(shader());

    wheel = new Model(wheelFile, false);
    wheel->shader(shader());

    return true;
}

void WindTurbine::update(float dtime)
{
    currentRotation = currentRotation + (-2 * M_PI / 6 * dtime);

    Matrix TM;
    TM.translation(this->spawnPosition);

    Matrix MoveWheel;
    MoveWheel.translation(spawnPosition.X, spawnPosition.Y + 5, spawnPosition.Z);

    Matrix RM;
    RM.rotationZ(currentRotation);
    pole->transform(TM);
    wheel->transform(MoveWheel * RM);

    // BoundingBoxen aktualisieren
    this->pole->BoundingBox.translate(spawnPosition);
    this->wheel->BoundingBox.translate(MoveWheel.translation());
}

void WindTurbine::draw(const BaseCamera& Cam)
{
    pole->draw(Cam);
    wheel->draw(Cam);
}
