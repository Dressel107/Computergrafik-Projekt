#include <stdio.h>
#include "../../src/Camera.h"
#include "../../src/Model.h"
#include <list>

class Upwind : public BaseModel
{
public:
    Upwind(float x, float y, float z, float height, float diameter, const char* file);
    virtual ~Upwind();
    bool loadModel(const char* file);
    void update(float dtime);
    virtual void draw(const BaseCamera& Cam);
    //const AABB& boundingBox() const { return capsule->boundingBox(); }
protected:
    float x;
    float y;
    float z;
    float nextPos = 0;
    typedef std::list<Model*> ModelList;
    ModelList WindDashes;
};
