#include "Glider.h"
#define _USE_MATH_DEFINES
#include <math.h>


Vector unitVecNegY = Vector(0, -1, 0);
Vector unitVecPosY = Vector(0, 1, 0);

Vector unitVecPosZ = Vector(0, 0, 1);

Vector unitVecPosX = Vector(1, 0, 0);
Vector unitVecNegX = Vector(-1, 0, 0);

const float maxVelocity = 18;
const float minVelocity = -1;


Glider::Glider(Vector spawnPosition)
{
	this->spawnPosition = spawnPosition;
	this->rotLeftRight = 0;
	this->rotUpDown = 0;
	this->currentWind = nullptr;
}

Glider::~Glider()
{
	delete glider;
}

bool Glider::loadModel(const char* gliderFile)
{
	glider = new Model(gliderFile, false);
	glider->shader(shader());

	//Gleiter an Starposition setzen
	Matrix TM;
	this->Transform = TM.translation(this->spawnPosition);
	glider->transform(this->Transform);
	this->glider->BoundingBox.transform(this->Transform);
	return true;
}

/// <summary>
///     Wird aufgerufen, wenn der Gleiter mit einem festen Gegenstand, bspw. dem Terrain kollidiert.
/// </summary>
void Glider::crash()
{
	this->isCrashed = true;
}

/// <summary>
///     Wird aufgerufen, wenn sich der Gleiter in einem Aufwind befindet.
/// </summary>

float distance = 0;
bool upwinded = false;

void Glider::upwind(Wind* wind)
{
	this->currentWind = wind;
	if (x == 0)
	{
		this->x = 1;
	}
	if (this->currentWind->isActiv == true) {
		distance = (this->transform().translation().Y - this->currentWind->transform().translation().Y);
		upwinded = true;
	}

	wind->trigger();
}


/// <summary>
///     Navigiert den Gleiter in eine bestimmte Richtung.
/// </summary>
/// <param name="UpDown">1.0f: Up, -1.0f: Down, 0.0f: Geradeaus</param>
/// <param name="LeftRight">1.0f: Right, -1.0f: Left, 0.0f: Geradeaus</param>
void Glider::navigate(float UpDown, float LeftRight)
{
	this->rotUpDown = UpDown;
	this->rotLeftRight = LeftRight;
}

Vector Glider::update(float dtime)
{
	// Wenn Gleiter gecrasht ist, auf Startposition zur�cksetzen
	if (this->isCrashed)
	{
		this->reset();
		this->isCrashed = false;
	}
	else
	{
		if (this->start == false)
		{
			return this->Transform.translation();
		}
		Matrix moveForwardMat, rotUpDownMat, rotLeftRightMat;
		calcNextMovment();

		//Kontinuierlich Geradeaus
		Vector gliderMovement = this->nextPos * dtime;
		moveForwardMat.translation(gliderMovement);

		//Neigen Oben/Unten
		rotUpDownMat.rotationX(this->nextRotX * dtime);

		//Neigen Links/Rechts
		rotLeftRightMat.rotationZ(this->nextRotZ * dtime);

		this->Transform = moveForwardMat * this->Transform * rotUpDownMat * rotLeftRightMat;
	}

	// Gleiter bewegen
	glider->transform(this->Transform);

	// BoundingBox aktualisieren
	this->glider->BoundingBox.transform(this->Transform);


	this->isInWind = false;
	return this->Transform.translation();
}

void Glider::draw(const BaseCamera& Cam)
{
	glider->draw(Cam);
}

//https://www.youtube.com/watch?v=uIgEwJVWVpY&t=368s&ab_channel=ChristopherScottVaughen
void Glider::calcNextMovment()
{
	float accKoe = 1;
	pitch = 0;


	//Aufsto� im Wind, St�rk abh�ngig von Abstand zischen Gleiter und Windboden
	if (upwinded == true) {
		accKoe =   (150/ distance);
		x = x + 0.02;
		lift = (accKoe * cos(x + M_PI) + accKoe);
		pitch -= (lift / 15);
		if (lift > -0.01 && lift < 0.01) {
			upwinded = false;
			x = 0;
			this->currentWind = nullptr;
		}
	}

	//Winkel zwischen Forward des Gleiters und Z-Achse
	float phi = atan2f(Transform.forward().normalize().Y, Transform.forward().normalize().Z);
	//Winkel zwischen Right des Gleiter und X-Achse
	float omega = atan2f(Transform.right().normalize().Y, Transform.right().normalize().X);


	//Beschleuning/Entschleuning 
	if (phi > 0) {
		velocity = velocity - (phi ) / 50;//Nach oben fliegen

	}
	else {
		velocity = (velocity - (((phi ) ) / 30));//Nach unten fliegen

	}

	if (phi < -M_PI / 2 && phi > M_PI / 2) {
		pitch = 0;
	}
	else {
		pitch += 0.03*(15 - velocity);
	}

	//Velocity auf Min und Max beschranken
	velocity = std::max(minVelocity, velocity);
	velocity = std::min(maxVelocity, velocity);

	//Abh�nigkeit von RotUp-Rotations-St�rke und Omega -> Nach oben Neigen schwer, nachen unten Neigen leicht
	float rotUpDownKoe = (2 + (abs(omega))) /6;







	this->nextPos = Transform.forward() * velocity + Transform.backward() * drag + unitVecPosY * lift + unitVecNegY * weight;
	this->nextRotX = (this->rotUpDown * rotUpDownKoe) + pitch;
	this->nextRotZ = this->rotLeftRight * 0.5;

}

void Glider::reset() {
	this->start = false;
	Matrix TM;
	this->Transform = TM.translation(this->spawnPosition);
	glider->transform(TM.translation(this->spawnPosition));
	this->glider->BoundingBox.transform(this->Transform);

	velocity = 8;
	pitch = 0;
	lift = 0.1;
	drag = 0.1;
	weight = 0.9;
	x = 1;

	this->currentWind = nullptr;

}


