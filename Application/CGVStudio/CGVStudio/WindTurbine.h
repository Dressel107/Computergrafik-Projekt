#include <stdio.h>
#include "../../src/Camera.h"
#include "../../src/Model.h"


class WindTurbine : public BaseModel
{
public:
    WindTurbine(Vector spawnPosition);
    virtual ~WindTurbine();
    bool loadModel(const char* poleFile, const char* wheelFile);
    void update(float dtime);
    virtual void draw(const BaseCamera& Cam);
    const AABB& boundingBoxPole() const { return pole->boundingBox(); }
    const AABB& boundingBoxWheel() const { return wheel->boundingBox(); }
protected:
    Model* pole;
    Model* wheel;
    Vector spawnPosition;
    float currentRotation = 0;
};
