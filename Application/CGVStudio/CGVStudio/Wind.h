#include <stdio.h>
#include "../../src/Camera.h"
#include "../../src/Model.h"
#include "../../src/PhongShader.h"

#pragma once
class Wind : public BaseModel
{
public:
    Wind(Vector spawnPosition);
    virtual ~Wind();
    bool loadModel(const char* file);
    void update(float dtime);
    virtual void draw(const BaseCamera& Cam);
    const AABB& boundingBox() const { return wind->BoundingBox; }
    void trigger();
    bool isActiv = true;
    void reset();
    PhongShader* pShader;


protected:
    Model* wind;
    Vector spawnPosition;
    float currentRotation = 0;
    float timer = 0;


};
