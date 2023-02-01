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

    sphere->transform(TM);
}

void Sphere::draw(const BaseCamera& Cam)
{
    sphere->draw(Cam);
}
