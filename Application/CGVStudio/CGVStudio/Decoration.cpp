#include "Decoration.h"
#define _USE_MATH_DEFINES
#include <math.h>

Decoration::Decoration(Vector spawnPosition)
{
    this->spawnPosition = spawnPosition;
}

Decoration::~Decoration()
{
    delete model;
}

bool Decoration::loadModel(const char* file)
{
    model = new Model(file, false);
    model->shader(shader());

    return true;
}

void Decoration::update(float dtime)
{
    Matrix Scale;
    Scale.scale(4);

    Matrix TM;
    TM.translation(this->spawnPosition);

    this->model->transform(TM * Scale);

    // BoundingBox aktualisieren
    this->model->BoundingBox.transform(this->model->transform());
}

void Decoration::draw(const BaseCamera& Cam)
{
    model->draw(Cam);
}
