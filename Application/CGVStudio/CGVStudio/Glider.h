#include <stdio.h>
#include "../../src/Camera.h"
#include "../../src/Model.h"

class Glider : public BaseModel
{
public:
    Glider(Vector spawnPosition);
    virtual ~Glider();
    bool loadModel(const char* gliderFile);
    Vector update(float dtime);

    virtual void draw(const BaseCamera& Cam);
    const AABB& boundingBox() const { return glider->boundingBox(); }
    void upwind();
    void crash();
    void navigate(float UpDown, float LeftRight);
    void navigateForTesting(float forwardBackward, float UpDown, float LeftRight);
    void startGlider() { start = true; };
protected:
    Model* glider;
    Vector spawnPosition;
    float currentPos = 0;

    float rotUpDown;
    float rotLeftRight;

    Vector movingVec;
    float flyPower;
    float rotationPower;
    
    Vector nextPos;
    float nextRot;
    void calcNextMovment();
    bool start = false;
    bool isCrashed = false;
};
