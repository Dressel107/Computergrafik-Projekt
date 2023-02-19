#include "Wind.h"
#define _USE_MATH_DEFINES
#include <math.h>


float upWindDuration = 20.0f; //In Sekunden

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

    return true;
}

void Wind::update(float dtime)
{
    if (timer == 0 || glfwGetTime() >= timer + upWindDuration) {
        isActiv = true;
    }


    this->currentRotation = this->currentRotation + (-2 * M_PI / 0.75 * dtime);

    Matrix TM;
    TM.translation(this->spawnPosition);

    Matrix RM;
    RM.rotationY(currentRotation);
    this->wind->transform(TM * RM);

    //BoundingBox aktualisieren
    this->wind->BoundingBox.transform(this->wind->transform());
}

void Wind::draw(const BaseCamera& Cam)
{
    this->wind->draw(Cam);
}

void Wind::trigger()
{
    if (isActiv == true)
    {
        timer = glfwGetTime();
        isActiv = false;
    }
}

void Wind::reset() {
    this->timer = 0;
    this->isActiv = true;
}
