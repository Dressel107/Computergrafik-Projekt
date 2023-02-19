#include "Capsule.h"
#define _USE_MATH_DEFINES
#include <math.h>

Capsule::Capsule(Vector spawnPosition)
{
    this->spawnPosition = spawnPosition;
}

Capsule::~Capsule()
{
    delete capsule;
}

bool Capsule::loadModel(const char* file)
{
    capsule = new Model(file, false);
    capsule->shader(shader());

    //capsule->transform( Scale);

    return true;
}

int Capsule::collect()
{
    return 20;
}

void Capsule::update(float dtime)
{
    this->currentRotation = this->currentRotation + (-2 * M_PI / 3 * dtime);

    Matrix Scale;
    Scale.scale(6);

    Matrix TM;
    TM.translation(this->spawnPosition);

    Matrix RM;
    RM.rotationY(currentRotation);
    this->capsule->transform(TM * RM * Scale);

    // BoundingBox aktualisieren
    this->capsule->BoundingBox.transform(this->capsule->transform());
}

void Capsule::draw(const BaseCamera& Cam)
{
    capsule->draw(Cam);
}
