#include <stdio.h>
#include "../../src/Camera.h"
#include "../../src/Model.h"
#include "Wind.h"

class Glider : public BaseModel
{
public:
    Glider(Vector spawnPosition);
    virtual ~Glider();
    bool loadModel(const char* gliderFile);
    Vector update(float dtime);

    virtual void draw(const BaseCamera& Cam);
    const AABB& boundingBox() const { return glider->boundingBox(); }
    void upwind(float dtime, Wind* wind);
    void crash();
    void navigate(float UpDown, float LeftRight);
    void navigateForTesting(float forwardBackward, float UpDown, float LeftRight);
    void startGlider() { start = true; };
    void reset();
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
    float nextRotX;
    float nextRotZ;
    void calcNextMovment();
    bool start = false;
    bool isCrashed = false;

    bool isInWind = false;
    Wind* currentWind;

    //void rek(float x, float distance);

};
