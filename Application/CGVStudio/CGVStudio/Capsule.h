#include <stdio.h>
#include "../../src/Camera.h"
#include "../../src/Model.h"
#include "ICollectable.h"

class Capsule : public BaseModel, public ICollectable
{
public:
    Capsule(Vector spawnPosition);
    virtual ~Capsule();
    bool loadModel(const char* file);
    void update(float dtime);
    virtual void draw(const BaseCamera& Cam);
    const AABB& boundingBox() const { return capsule->boundingBox(); }
    virtual int collect();
protected:
    Model* capsule;
    Vector spawnPosition;
    float currentRotation = 0;
};
