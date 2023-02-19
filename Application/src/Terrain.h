#ifndef Terrain_hpp
#define Terrain_hpp

#include <stdio.h>
#include "basemodel.h"
#include "texture.h"
#include "vertexbuffer.h"
#include "indexbuffer.h"
#include "Aabb.h"
#include "../CGVStudio/CGVStudio/TerrainArea.h"

class Terrain : public BaseModel
{
private:
    float getGrayValue(const Color& pixelColor);
    Vector& getVertex(int x, int y);

public:
    Terrain(const char* HeightMap=NULL, const char* DetailMap1=NULL, const char* DetailMap2=NULL);
    virtual ~Terrain();
    bool load(const char* HeightMap, const char* DetailMap1, const char* DetailMap2, float terrainScaling);

    virtual void shader( BaseShader* shader, bool deleteOnDestruction=false );
    virtual void draw(const BaseCamera& Cam);
    
    float width() const { return Size.X; }
    float height() const { return Size.Y; }
    float depth() const { return Size.Z; }

    void width(float v) { Size.X = v; }
    void height(float v) { Size.Y = v; }
    void depth(float v) { Size.Z = v; }

    Vector normal(const Vector&, const Vector&, const Vector&);
    
    const Vector& size() const { return Size; }
    void size(const Vector& s) {
        Size = s;
    }

    void setK(int kValue) { k = kValue; }

    bool intersectWith(AABB aabb);

    Vector* tmpVertices;
    int imgWidth;
    int imgHeight;
protected:
    void applyShaderParameter();
    
    VertexBuffer VB;
    IndexBuffer IB;
    Texture DetailTex[2];
    Texture MixTex;
    Texture HeightTex;
    Vector Size;
    int k;
    float terrainScaling;

    const int areasCount = 64;
    const int areasPerAxis = 8;
    TerrainArea areas[64];
};



#endif /* Terrain_hpp */
