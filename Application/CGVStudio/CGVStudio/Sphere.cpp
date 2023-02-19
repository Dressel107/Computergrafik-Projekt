#include "Sphere.h"
#define _USE_MATH_DEFINES
#include <math.h>

Sphere::Sphere(Vector spawnPosition)
{
    this->spawnPosition = spawnPosition;
}

Sphere::~Sphere()
{
    delete sphere;
}

bool Sphere::loadModel(const char* file)
{

    sphere = new Model(file, false);
    sphere->shader(shader());
    Matrix Scale;
    Scale.scale(6);
    sphere->transform(Scale);
    // BoundingBox aktualisieren

    this->sphere->BoundingBox.translate(spawnPosition);

    return true;
}

int Sphere::collect()
{
    return 10;
}

void Sphere::update(float dtime)
{
    Matrix TM;
    TM.translation(this->spawnPosition);

    this->sphere->transform(TM);

}

void Sphere::draw(const BaseCamera& Cam)
{
    sphere->draw(Cam);
}
