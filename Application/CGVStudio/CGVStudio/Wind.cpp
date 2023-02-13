#include "Wind.h"
#define _USE_MATH_DEFINES
#include <math.h>

Wind::Wind(Vector spawnPosition)
{
    this->spawnPosition = spawnPosition;
}

Wind::~Wind()
{
    delete wind;
}

bool Wind::loadModel(const char* file)
{
    this->wind = new Model(file, false);
    this->wind->shader(shader());

    // BoundingBox aktualisieren
    this->wind->BoundingBox.translate(spawnPosition);

    return true;
}

void Wind::update(float dtime)
{
    currentRotation = currentRotation + (-2 * M_PI / 0.5 * dtime);

    Matrix TM;
    TM.translation(this->spawnPosition);

    Matrix RM;
    RM.rotationY(currentRotation);
    wind->transform(TM * RM);

    // BoundingBox aktualisieren
    wind->BoundingBox.translate(spawnPosition);
}

void Wind::draw(const BaseCamera& Cam)
{
    wind->draw(Cam);
}
