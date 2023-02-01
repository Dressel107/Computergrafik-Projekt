#include <stdio.h>
#include "../../src/Camera.h"
#include "../../src/Model.h"
#include "ICollectable.h"

#pragma once
class Sphere : public BaseModel, public ICollectable
{
public:
    Sphere(Vector spawnPosition);
    virtual ~Sphere();
    bool loadModel(const char* file);
    void update(float dtime);
    virtual void draw(const BaseCamera& Cam);
    const AABB& boundingBox() const { return sphere->boundingBox(); }
    virtual int collect();
protected:
    Model* sphere;
    Vector spawnPosition;
};
