#include <stdio.h>
#include "../../src/Camera.h"
#include "../../src/Model.h"
#include "ICollectable.h"

#pragma once
class SpeedDisc : public BaseModel, public ICollectable
{
public:
    enum SpeedMode { Min30, Min50, Min75, Min90, Min110 };

    SpeedDisc(Vector spawnPosition);
    virtual ~SpeedDisc();
    bool loadModel(SpeedMode speedMode);
    void update(float dtime);
    virtual void draw(const BaseCamera& Cam);
    const AABB& boundingBox() const { return speedDisc->boundingBox(); }
    virtual int collect();
protected:
    Model* speedDisc;
    Vector spawnPosition;
    int pointsWhenCollect = 0;
};

