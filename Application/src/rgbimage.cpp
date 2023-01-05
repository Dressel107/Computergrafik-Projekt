#include "rgbimage.h"
#include "color.h"
#include "assert.h"

const int BYTES_PER_PIXEL = 3;
const int FILE_HEADER_SIZE = 14;
const int INFO_HEADER_SIZE = 40;

RGBImage::RGBImage(unsigned int Width, unsigned int Height)
{
    m_Image = new Color[Width * Height];
    m_Height = Height;
    m_Width = Width;
}

RGBImage::~RGBImage()
{
    delete[] m_Image;
}

// https://www.mathematik.uni-marburg.de/~thormae/lectures/graphics1/graphics_3_1_ger_web.html#1
void RGBImage::setPixelColor(unsigned int x, unsigned int y, const Color& c)
{
    m_Image[y * m_Width + x] = c;
}

// https://www.mathematik.uni-marburg.de/~thormae/lectures/graphics1/graphics_3_1_ger_web.html#1
const Color& RGBImage::getPixelColor(unsigned int x, unsigned int y) const
{
    return m_Image[y * m_Width + x];
}

unsigned int RGBImage::width() const
{
    return m_Width;
}
unsigned int RGBImage::height() const
{
    return m_Height;
}

unsigned char RGBImage::convertColorChannel(float v)
{
    if (v < 0.0f)
    {
        v = 0.0f;
    }
    else if (v > 1.0f)
    {
        v = 1.0f;
    }

    return 255 * v;
}

bool RGBImage::saveToDisk(const char* Filename)
{
    int fileSize = FILE_HEADER_SIZE + INFO_HEADER_SIZE + (m_Width * m_Height * BYTES_PER_PIXEL);

    // https://stackoverflow.com/questions/2654480/writing-bmp-image-in-pure-c-c-without-other-libraries
    static unsigned char fileHeader[] =
    {
        0,0,     /// signature
        0,0,0,0, /// image file size in bytes
        0,0,0,0, /// reserved
        0,0,0,0, /// start of pixel array
    };

    fileHeader[0] = (unsigned char)('B');
    fileHeader[1] = (unsigned char)('M');
    fileHeader[2] = (unsigned char)(fileSize);
    fileHeader[3] = (unsigned char)(fileSize >> 8);
    fileHeader[4] = (unsigned char)(fileSize >> 16);
    fileHeader[5] = (unsigned char)(fileSize >> 24);
    fileHeader[10] = (unsigned char)(FILE_HEADER_SIZE + INFO_HEADER_SIZE);

    static unsigned char infoHeader[] =
    {
        0,0,0,0, /// header size
        0,0,0,0, /// image width
        0,0,0,0, /// image height
        0,0,     /// number of color planes
        0,0,     /// bits per pixel
        0,0,0,0, /// compression
        0,0,0,0, /// image size
        0,0,0,0, /// horizontal resolution
        0,0,0,0, /// vertical resolution
        0,0,0,0, /// colors in color table
        0,0,0,0, /// important color count
    };

    infoHeader[0] = (unsigned char)(INFO_HEADER_SIZE);
    infoHeader[4] = (unsigned char)(m_Width);
    infoHeader[5] = (unsigned char)(m_Width >> 8);
    infoHeader[6] = (unsigned char)(m_Width >> 16);
    infoHeader[7] = (unsigned char)(m_Width >> 24);
    infoHeader[8] = (unsigned char)(m_Height);
    infoHeader[9] = (unsigned char)(m_Height >> 8);
    infoHeader[10] = (unsigned char)(m_Height >> 16);
    infoHeader[11] = (unsigned char)(m_Height >> 24);
    infoHeader[12] = (unsigned char)(1);
    infoHeader[14] = (unsigned char)(BYTES_PER_PIXEL * 8);

    FILE* file = fopen(Filename, "wb");

    if (file == NULL)
    {
        return false;
    }

    fwrite(&fileHeader, FILE_HEADER_SIZE, 1, file);
    fwrite(&infoHeader, INFO_HEADER_SIZE, 1, file);

    for (int y = m_Height - 1; y >= 0; y--)
    {
        for (int x = 0; x < m_Width; x++)
        {
            // Hier RGB umgekehrt in Datei schreiben
            char b = convertColorChannel(getPixelColor(x, y).B);
            char g = convertColorChannel(getPixelColor(x, y).G);
            char r = convertColorChannel(getPixelColor(x, y).R);

            fwrite(&b, 1, sizeof(b), file);
            fwrite(&g, 1, sizeof(g), file);
            fwrite(&r, 1, sizeof(r), file);
        }
    }

    fclose(file);
    return true;
}

RGBImage& RGBImage::SobelFilter(RGBImage& dst, const RGBImage& src, float factor) 
{
    // Prüfen, ob die Bilder dieselben Maße besitzen
    assert(src.width() == dst.width() && src.height() == dst.height());

    float K[3][3] = { {1.0f, 0.0f, -1.0f},
                      {2.0f, 0.0f, -2.0f},
                      {1.0f, 0.0f, -1.0f} };

    float KTransponiert[3][3] = { {1.0f, 2.0f, 1.0f},
                                {0.0f, 0.0f, 0.0f},
                                {-1.0f, -2.0f, -1.0f} };

    // Alle Pixel des Bildes durchlaufen
    for (int x = 0; x < src.width(); x++)
    {
        for (int y = 0; y < src.height(); y++)
        {
            Color u = Color();
            Color v = Color();

            // U und V berechnen (Die beiden Summenformeln)
            for (int i = 0; i <= 2; i++)
            {
                for (int j = 0; j <= 2; j++)
                {
                    if (x - i - 1 >= 0 && y - j - 1 >= 0)
                    {
                        Color currentPixelColor = src.getPixelColor(x - i - 1, y - j - 1);

                        u += currentPixelColor * K[i][j];
                        v += currentPixelColor * KTransponiert[i][j];
                    }
                }
            }

            // S berechnen (Neue Pixelfarbe)
            Color s = (u * u) + (v * v);
            s = Color(sqrt(s.R), sqrt(s.G), sqrt(s.B));
            dst.setPixelColor(x, y, s * factor);
        }
    }

    return dst;
}