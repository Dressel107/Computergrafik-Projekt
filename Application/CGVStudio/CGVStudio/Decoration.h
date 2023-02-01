#include <stdio.h>
#include "../../src/Camera.h"
#include "../../src/Model.h"

#pragma once
class Decoration : public BaseModel
{
public:
    Decoration(Vector spawnPosition);
    virtual ~Decoration();
    bool loadModel(const char* file);
    void update(float dtime);
    virtual void draw(const BaseCamera& Cam);
    const AABB& boundingBox() const { return model->boundingBox(); }
protected:
    Model* model;
    Vector spawnPosition;
    float currentRotation = 0;
};
