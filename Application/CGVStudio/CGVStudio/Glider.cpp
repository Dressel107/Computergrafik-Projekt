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
	this->glider->BoundingBox.translate(spawnPosition);
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

void Glider::upwind(float dtime, Wind* wind)
{
	this->isInWind = true;
	this->currentWind = wind;
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
	// Wenn Gleiter gecrasht ist, auf Startposition zurücksetzen
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




float velocity = 8;
float pitch = 0;

float lift = 0.1;
float drag = 0.01;
float weight = 0.9;

float x = 1;


//https://www.youtube.com/watch?v=uIgEwJVWVpY&t=368s&ab_channel=ChristopherScottVaughen
void Glider::calcNextMovment()
{
	float accKoe = 1;

	//Aufstoß im Wind, Stärk abhängig von Abstand zischen Gleiter und Windboden
	//if (this->currentWind != nullptr && this->currentWind->isActiv == false) {
	//	//distance = (this->transform().translation().Y - this->currentWind->transform().translation().Y);
	//	 //lift = 20000 / (distance * distance);
	//	 //this->currentWind->trigger();
	//}
	//else {
	//	lift = 0.1;
	//}

	//if (this->currentWind != nullptr && this->currentWind->isActiv == true) {
	//	distance = (this->transform().translation().Y - this->currentWind->transform().translation().Y);
	//}
	//else {
	//	distance = 0;
	//}

	//accKoe = 0.1 * distance;
	//x = x + 0.1;
	//lift = (accKoe * (cos(x + M_PI) + accKoe)) / 5;

	if (upwinded == true) {
		accKoe =   (150/ distance);
		x = x + 0.02;
		lift = (accKoe * cos(x + M_PI) + accKoe);
		if (lift > -0.01 && lift < 0.01) {
			upwinded = false;
			x = 0;
			std::cout << "end" << std::endl;
		}
	}


	
	std::cout << accKoe << "|" << x << "|" << lift << std::endl;



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
		//pitch = 1 / ((abs(velocity) + 1) * 5);
		pitch = 0.03*(15 - velocity);

	}

	//Velocity auf Min und Max beschranken
	velocity = std::max(minVelocity, velocity);
	velocity = std::min(maxVelocity, velocity);

	//Abhänigkeit von RotUp-Rotations-Stärke und Omega -> Nach oben Neigen schwer, nachen unten Neigen leicht
	float rotUpDownKoe = (1 + (abs(omega))) / 4;

	//Abhänigkeit von LeftRight Rotation und Oegma -> Gleiter wird immer wieder waagerecht
	//float rotLeftRightKoe = -(omega * abs(omega) / 4);

	this->nextPos = Transform.forward() * velocity + Transform.backward() * drag + unitVecPosY * lift + unitVecNegY * weight;
	this->nextRotX = (this->rotUpDown * rotUpDownKoe) + pitch;
	this->nextRotZ = this->rotLeftRight;// +rotLeftRightKoe;

	//std::cout << velocity << "|" << pitch << "|" << phi << "|" << (omega * abs(omega)) << std::endl;
}

void Glider::reset() {
	this->start = false;
	//Gleiter an Starposition setzen
	Matrix TM;
	this->Transform = TM.translation(this->spawnPosition);
	glider->transform(this->Transform);
	velocity = 5;
	pitch = 0;
	lift = 0.1;
	upwinded = false;

	drag = 0.0000324;
	weight = 0.9317522032;
	if (this->currentWind != nullptr) {
		this->currentWind->isActiv = true;
	}
	this->currentWind = nullptr;

	this->glider->BoundingBox.translate(spawnPosition);
}


