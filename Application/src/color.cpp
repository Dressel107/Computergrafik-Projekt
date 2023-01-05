#include "color.h"
#include <assert.h>

Color::Color()
{
	R = 0;
	G = 0;
	B = 0;
}

Color::Color(float r, float g, float b)
{
	//if (r < 0)
	//{
	//	r = 0;
	//}
	//else if (r > 255)
	//{
	//	r = 255;
	//}

	//if (g < 0)
	//{
	//	g = 0;
	//}
	//else if (g > 255)
	//{
	//	g = 255;
	//}

	//if (b < 0)
	//{
	//	b = 0;
	//}
	//else if (b > 255)
	//{
	//	b = 255;
	//}

	R = r;
	G = g;
	B = b;
}

Color Color::operator*(const Color& c) const
{
	return Color(R * c.R, G * c.G, B * c.B);
}

Color Color::operator*(const float Factor) const
{
	return Color(R * Factor, G * Factor, B * Factor);
}

Color Color::operator+(const Color& c) const
{
	return Color(R + c.R, G + c.G, B + c.B);
}

Color& Color::operator+=(const Color& c)
{
	R += c.R;
	G += c.G;
	B += c.B;

	return *this;
}

