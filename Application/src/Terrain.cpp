#include "Terrain.h"
#include "rgbimage.h"
#include "Terrainshader.h"


Terrain::Terrain(const char* HeightMap, const char* DetailMap1, const char* DetailMap2) : Size(10,1,10)
{
    if(HeightMap && DetailMap1 && DetailMap2)
    {
        bool loaded = load( HeightMap, DetailMap1, DetailMap2, 1);
        if(!loaded)
            throw std::exception();
    }   
}

Terrain::~Terrain()
{
    delete[] tmpVertices;
}

Vector Terrain::normal(const Vector& a, const Vector& b, const Vector& c) 
{

    Vector triangleNormal = (b - a).cross(c - a);
    if (triangleNormal.X == 0 && triangleNormal.Y == 0 && triangleNormal.Z == 0) {
        return Vector(0, 0, 0);
    }
    return triangleNormal.normalize();
}

Vector& Terrain::getVertex(int x, int y)
{
    return this->tmpVertices[y * this->imgWidth + x];
}

bool Terrain::load(const char* HeightMap, const char* DetailMap1, const char* DetailMap2, float terrainScaling)
{
    this->terrainScaling = terrainScaling;

    if( !HeightTex.load(HeightMap) )
        return false;
    if( !DetailTex[0].load(DetailMap1) )
        return false;
    if( !DetailTex[1].load(DetailMap2) )
        return false;
    
    // TODO: add code for creating terrain model
    const RGBImage* img = HeightTex.getRGBImage();

    // Aufgabe 2
    RGBImage mixMap = RGBImage(img->width(), img->height());
    RGBImage::SobelFilter(mixMap, *img, 10);
    mixMap.saveToDisk("MixMap.bmp");

    // Aufgabe 3
    MixTex.load("MixMap.bmp");

    this->imgWidth = img->width();
    this->imgHeight = img->height();

    float scaleX = Size.X / imgWidth;
    float scaleY = Size.Z / imgHeight;

    //Temporäres 2D Verticesarray erzeugen, welches ermöglicht die Nachbarn eines Vertices zu bestimmen.
    //Size von tmpVertices ist imgWidth + 2 und imgHeight + 2 um bei der Nachbarsuche Nullpointer zu vermeiden.
    this->tmpVertices = new Vector[imgWidth * imgHeight];

    //Temporäres 2D Verticesarray befüllen
    for (int x = 0; x < imgWidth; x++) {
        for (int y = 0; y < imgHeight; y++)
        {
            tmpVertices[(y) * imgWidth + (x )] = Vector(-width() / 2.0 + x * scaleX, getGrayValue(img->getPixelColor(x, y)), depth() / 2.0 - y * scaleY);
        }
    }

    VB.begin();
    for (int x = 0; x < imgWidth; x++) {
        for (int y = 0; y < imgHeight; y++)
        {
            //Im 2D Nachbar-Vertices finden, daraus Dreieck bilden und Normale des Dreiecks aufaddieren, siehe this->normal()
            Vector normal = Vector(0,0,0);
            if (x != 0 && y != 0 && x != imgWidth - 1 && y != imgHeight - 1) {
                 normal += this->normal(getVertex(x, y), getVertex(x - 1, y), getVertex(x - 1, y - 1));
                 normal += this->normal(getVertex(x, y), getVertex(x - 1, y - 1), getVertex(x, y - 1));
                 normal += this->normal(getVertex(x, y), getVertex(x, y - 1), getVertex(x + 1, y));
                 normal += this->normal(getVertex(x, y), getVertex(x + 1, y), getVertex(x + 1, y + 1));
                 normal += this->normal(getVertex(x, y), getVertex(x + 1, y + 1), getVertex(x, y + 1));
                 normal += this->normal(getVertex(x, y), getVertex(x, y + 1), getVertex(x - 1, y));
            }

            VB.addNormal(normal.normalize());

            VB.addTexcoord0(x / (float)  (imgWidth - 1), y / (float) (imgHeight - 1));

            float grayValue = getGrayValue(img->getPixelColor(x, y));
            Vector pos = Vector(-width() / 2.0 + x * scaleX, grayValue, depth() / 2.0 - y * scaleY);
            VB.addVertex(pos);
        }
    }
    VB.end();

    IB.begin();
    for (int i = 0; i < imgHeight * imgWidth - imgHeight - 1; i++)
    {
        if (i != 0 && ((i + 1) % imgHeight == 0)) {
            i++;
        }

        IB.addIndex(i + imgHeight + 1);
        IB.addIndex(i + 1);
        IB.addIndex(i);

        IB.addIndex(i);
        IB.addIndex(i + imgHeight);
        IB.addIndex(i + imgHeight + 1);
    }
    IB.end();

    // TerrainAreas erzeugen
    // (Für Kollisionserkennung)
    int areaCount = 0;
    for (int i = 0; i < areasPerAxis; i++)
    {
        for (int j = 0; j < areasPerAxis; j++)
        {
            TerrainArea area;
            area.vertices = new Vector[(imgWidth / areasPerAxis) * (imgHeight / areasPerAxis)];

            // Defaultwerte setzen
            area.boundingbox.Min.X = 1000000;
            area.boundingbox.Min.Y = 1000000;
            area.boundingbox.Min.Z = 1000000;
            area.boundingbox.Max.X = -1000000;
            area.boundingbox.Max.Y = -1000000;
            area.boundingbox.Max.Z = -1000000;

            int vertexCount = 0;

            // BoundingBoxen um TerrainBereiche lege
            for (int x = (imgWidth / areasPerAxis) * i; x < (imgWidth / areasPerAxis) * (i + 1); x++)
            {
                for (int y = (imgWidth / areasPerAxis) * j; y < (imgHeight / areasPerAxis) * (j + 1); y++)
                {
                    Vector v = tmpVertices[(y)*imgWidth + (x)];
                    v = v * this->terrainScaling;

                    area.vertices[vertexCount] = v;
                    vertexCount++;

                    // X
                    if (v.X < area.boundingbox.Min.X)
                    {
                        area.boundingbox.Min.X = v.X;
                    }
                    if (v.X > area.boundingbox.Max.X)
                    {
                        area.boundingbox.Max.X = v.X;
                    }

                    // Y
                    if (v.Y < area.boundingbox.Min.Y)
                    {
                        area.boundingbox.Min.Y = v.Y;
                    }
                    if (v.Y > area.boundingbox.Max.Y)
                    {
                        area.boundingbox.Max.Y = v.Y;
                    }

                    // Z
                    if (v.Z < area.boundingbox.Min.Z)
                    {
                        area.boundingbox.Min.Z = v.Z;
                    }
                    if (v.Z > area.boundingbox.Max.Z)
                    {
                        area.boundingbox.Max.Z = v.Z;
                    }
                }
            }

            this->areas[areaCount] = area;
            areaCount++;
        }
    }
    
    return true;
}

/// <summary>
///     Prüft, ob die übergebene AABB das Terrain berührt.
/// </summary>
bool Terrain::intersectWith(AABB aabb)
{
    for (int i = 0; i < areasCount; i++)
    {
        TerrainArea area = areas[i];

        // Prüfen, ob die AABB einen Terrainbereich berührt
        if (area.boundingbox.intersectWith(aabb))
        {
            // Alle Vertices des Terrainbereichs prüfen
            for (int x = 0; x < (imgWidth / areasPerAxis) * (imgHeight / areasPerAxis); x++)
            {
                Vector v = area.vertices[x];

                if (aabb.intersectWith(v))
                {
                    return true;
                }
            }
        }
    }

    return false;
}


/*
bool Terrain::load(const char* HeightMap, const char* DetailMap1, const char* DetailMap2)
{
    if (!HeightTex.load(HeightMap))
        return false;
    if (!DetailTex[0].load(DetailMap1))
        return false;
    if (!DetailTex[1].load(DetailMap2))
        return false;


    const RGBImage* img = HeightTex.getRGBImage();


    //Exercise 2
    RGBImage mixMap = RGBImage(img->width(), img->height());
    RGBImage::SobelFilter(mixMap, *img, 10);
    mixMap.saveToDisk("MixMap.bmp");


    //Exercise 3
    MixTex.load("MixMap.bmp");


    int imgWidth = img->width();
    int imgHeight = img->height();

    float scaleX = Size.X / imgWidth;
    float scaleY = Size.Z / imgHeight;

    VB.begin();
    for (int x = 0; x < imgWidth; x++) {
        for (int y = 0; y < imgHeight; y++)
        {
            float grayScaleValue = getGrayValue(img->getPixelColor(x, y));

            Vector pos = Vector(-width() / 2.0 + x * scaleX, grayScaleValue, depth() / 2.0 - y * scaleY);

            //VB.addNormal(getNormal(pos, x, y, img));

            VB.addTexcoord0(x / (float)(imgWidth - 1), y / (float)(imgHeight - 1));

            VB.addVertex(pos);
        }
    }
    VB.end();

    IB.begin();
    for (int i = 0; i < imgHeight * imgWidth - imgHeight - 1; i++)
    {
        if (i != 0 && ((i + 1) % imgHeight == 0)) {
            i++;
        }

        IB.addIndex(i + imgHeight + 1);
        IB.addIndex(i + 1);
        IB.addIndex(i);

        IB.addIndex(i);
        IB.addIndex(i + imgHeight);
        IB.addIndex(i + imgHeight + 1);
    }
    IB.end();

    return true;
}
*/

float Terrain::getGrayValue(const Color& pixelColor) {
    //Quelle: https://stackoverflow.com/questions/15972490/bufferedimage-getting-the-value-of-a-pixel-in-grayscale-color-model-picture

    float value = pixelColor.R + pixelColor.G + pixelColor.B;
    value = value / 3;

    return value;
}

void Terrain::shader( BaseShader* shader, bool deleteOnDestruction )
{
    BaseModel::shader(shader, deleteOnDestruction);
}

// Fertig
void Terrain::draw(const BaseCamera& Cam)
{
    applyShaderParameter();
    BaseModel::draw(Cam);

    VB.activate();
    IB.activate();

    glDrawElements(GL_TRIANGLES, IB.indexCount(), IB.indexFormat(), 0);

    IB.deactivate();
    VB.deactivate();
}

void Terrain::applyShaderParameter()
{
    TerrainShader* Shader = dynamic_cast<TerrainShader*>(BaseModel::shader());
    if(!Shader)
        return;
    
    Shader->mixTex(&MixTex);
    for(int i=0; i<2; i++)
        Shader->detailTex(i,&DetailTex[i]);
    Shader->scaling(Size);
    Shader->setK(k);
    
    // TODO: add additional parameters if needed..
}


