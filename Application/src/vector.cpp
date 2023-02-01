#include "vector.h"
#include <assert.h>
#include <math.h>

#define EPSILON 1e-6

Vector::Vector( float x, float y, float z)
{
	this->X = x;
	this->Y = y;
	this->Z = z;
}

Vector::Vector()
{
}

float Vector::dot(const Vector& v) const
{
	return this->X * v.X + this->Y * v.Y + this->Z * v.Z;
}

Vector Vector::cross(const Vector& v) const
{
	return Vector((this->Y * v.Z - this->Z * v.Y), (this->Z * v.X - this->X * v.Z), (this->X * v.Y - this->Y * v.X)); // dummy (remove)
}

Vector Vector::operator+(const Vector& v) const
{
	
	return Vector(this->X + v.X, this->Y + v.Y, this->Z + v.Z); 
}

Vector Vector::operator-(const Vector& v) const
{
	return Vector(this->X - v.X, this->Y - v.Y, this->Z - v.Z);
}

Vector Vector::operator*(float c) const
{
	return Vector(this->X * c, this->Y * c, this->Z * c); 
}

Vector Vector::operator-() const
{

	return Vector(this->X * -1, this->Y * -1, this->Z* -1); 
}

Vector& Vector::operator+=(const Vector& v)
{
	this->X = this->X + v.X;
	this->Y = this->Y + v.Y;
	this->Z = this->Z + v.Z;

	return *this; 
}

Vector& Vector::normalize()
{
	*this = *this * (1 / this->length());
	return *this; 
}

float Vector::length() const
{
	return sqrt(this->lengthSquared()); 
}

float Vector::lengthSquared() const
{
	return this-> X * this->X + this->Y * this->Y + this->Z * this->Z; 
}

Vector Vector::reflection( const Vector& normal) const
{
	Vector vec = *this + normal  * (-2.0 * (this->dot(normal)));
	//Vector vec = *this * (-2.0 * (this->dot(normal) * normal));

	return vec;
}
/* Liefert true wenn Strahl d das Dreieck abc trifft.
*  Strahl d hat den Ursprung (*this)
*  Auftreffpunkt = (*this) + d * s
*/
bool Vector::triangleIntersection( const Vector& d, const Vector& a, const Vector& b, const Vector& c, float& s) const
{
	//1. Schnittpunkt mit der Ebene bestimmen in der das Dreieck liegt
	//1.1 Normale der Ebene berechenen
	//Vector n = (b - a).cross(c-a) * (1 / (b-a).cross(c-a).length())
	Vector n = (b - a).cross(c - a).normalize();

	//1.2 Abstand dis mithilfe der Ebenegleich berechenen
	float distance = n.dot(a);

	//1.3 Skalierung s
	s = (distance - n.dot(*this)) / n.dot(d);

	if (s < EPSILON) {
		return false; //Ebene liegt hinter dem Strahl
	}
	//Intersectionpoint mit der Ebene
	Vector p = *this + d * s;

	//2. Strahl-Dreieck-Kollisionstest
	float abc = ((b - a).cross((c - a)).length()) / 2.0;

	float abp = ((b - a).cross((p - a)).length()) / 2.0;
	float acp = ((c - a).cross((p - a)).length()) / 2.0;
	float bcp = ((c - b).cross((p - b)).length()) / 2.0;

	if (abc + EPSILON >= abp + acp + bcp) {
		return true;
	}

	return false; 
}
