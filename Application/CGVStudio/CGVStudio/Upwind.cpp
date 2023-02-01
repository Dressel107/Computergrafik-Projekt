#include "Upwind.h"
#define _USE_MATH_DEFINES
#include <math.h>

Upwind::Upwind(float x, float y, float z, float height, float diameter, const char* file)
{
    this->x = x;
    this->y = y;
    this->z = z;

    for (int i = 0; i < (int)height; i++)
    {
        Model* windDash = new Model(file, false);
        windDash->shader(shader());
        WindDashes.push_back(windDash);
    }
}

Upwind::~Upwind()
{
    //delete capsule;
}

bool Upwind::loadModel(const char* file)
{
    return true;
}

void Upwind::update(float dtime)
{
    nextPos = nextPos + (-2 * M_PI / 4 * dtime);
    int counter = y;

    for each (Model* dash in WindDashes)
    {
        Matrix moveFirst;
        moveFirst.translation(1, y, 1);

        Matrix TM;
        TM.translation(this->x, this->y, this->z);

        Matrix RM;
        RM.rotationY(nextPos);

        dash->transform(TM * RM * moveFirst);
        counter++;
    }
}

void Upwind::draw(const BaseCamera& Cam)
{
    for each (Model* dash in WindDashes)
    {
        dash->draw(Cam);
    }
}
