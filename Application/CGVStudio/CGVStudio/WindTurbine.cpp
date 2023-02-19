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

    Matrix Scale;
    Scale.scale(4);

    Matrix TM;
    TM.translation(this->spawnPosition);

    Matrix MoveWheel;
    MoveWheel.translation(spawnPosition.X, spawnPosition.Y + 20, spawnPosition.Z);

    Matrix RM;
    RM.rotationZ(currentRotation);
    pole->transform(TM * Scale);
    wheel->transform(MoveWheel * RM * Scale);

    // BoundingBoxen aktualisieren
    this->pole->BoundingBox.transform(this->pole->transform());
    this->wheel->BoundingBox.transform(this->wheel->transform());
}

void WindTurbine::draw(const BaseCamera& Cam)
{
    pole->draw(Cam);
    wheel->draw(Cam);
}
