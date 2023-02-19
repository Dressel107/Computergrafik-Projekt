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
    Matrix Scale;
    Scale.scale(20);
    //capsule->transform( Scale);

    return true;
}

int Capsule::collect()
{
    return 20;
}

void Capsule::update(float dtime)
{
    currentRotation = currentRotation + (-2 * M_PI / 3 * dtime);



    Matrix TM;
    TM.translation(this->spawnPosition);

    Matrix RM;
    RM.rotationY(currentRotation);
    capsule->transform(TM * RM);

    // BoundingBox aktualisieren
    this->capsule->BoundingBox.transform(this->capsule->transform());
}

void Capsule::draw(const BaseCamera& Cam)
{
    capsule->draw(Cam);
}
