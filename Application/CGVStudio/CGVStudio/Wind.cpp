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
        //std::cout << this->pShader->lightColor().R << "|" << this->pShader->lightColor().G <<"|" << this->pShader->lightColor().B << std::endl;
        this->pShader->lightColor(Color(1, 1, 1));

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
        this->pShader->lightColor(Color(255, 0, 0));
    }
}

void Wind::reset() {
    this->timer = 0;
    this->isActiv = true;

}

