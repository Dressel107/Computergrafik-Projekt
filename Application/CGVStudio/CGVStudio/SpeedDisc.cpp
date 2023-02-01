#include "SpeedDisc.h"
#define _USE_MATH_DEFINES
#define ASSET_DIRECTORY "../../assets/"
#include <math.h>

SpeedDisc::SpeedDisc(Vector spawnPosition)
{
    this->spawnPosition = spawnPosition;
}

SpeedDisc::~SpeedDisc()
{
    delete speedDisc;
}

bool SpeedDisc::loadModel(SpeedMode speedMode)
{
    switch (speedMode)
    {
        case SpeedDisc::Min30:
            speedDisc = new Model(ASSET_DIRECTORY "speed_disc_30.dae", false);      
            pointsWhenCollect = 5;
            break;
        case SpeedDisc::Min50:
            speedDisc = new Model("speed_disc_30.dae", false);
            pointsWhenCollect = 10;
            break;
        case SpeedDisc::Min75:
            speedDisc = new Model("speed_disc_30.dae", false);
            pointsWhenCollect = 15;
            break;
        case SpeedDisc::Min90:
            speedDisc = new Model("speed_disc_30.dae", false);
            pointsWhenCollect = 20;
            break;
        case SpeedDisc::Min110:
            speedDisc = new Model("speed_disc_30.dae", false);
            pointsWhenCollect = 25;
            break;
        default:
            break;
    }

    speedDisc->shader(shader());
    return true;
}

int SpeedDisc::collect()
{
    return this->pointsWhenCollect;
}

void SpeedDisc::update(float dtime)
{
    Matrix TM;
    TM.translation(this->spawnPosition);
    speedDisc->transform(TM);
}

void SpeedDisc::draw(const BaseCamera& Cam)
{
    speedDisc->draw(Cam);
}
