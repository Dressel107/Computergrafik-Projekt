#include "Glider.h"
#define _USE_MATH_DEFINES
#include <math.h>


Vector unitVecNegY = Vector(0, -1, 0);
Vector unitVecPosY = Vector(0, 1, 0);

Vector unitVecPosZ = Vector(0, 0, 1);
Vector unitVecPosX = Vector(1, 0, 0);
Vector unitVecNegX = Vector(-1, 0, 0);



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

void Glider::upwind(float dtime, Wind* wind)
{
	this->isInWind = true;
	this->currentWind = wind;
	wind->trigger();
	
}

//void Glider::rek(float x, float distance) {
//    if (x == 0) {
//        x = 10 * cos(distance * 0.05) + 10;
//        tmp = distance;
//    }
//    else {
//        x = 10 * cos((tmp) * 0.05) + 10;
//        tmp = tmp + 1;
//
//    }
//
//    if (x < 1) {
//        x = 0;
//        return;
//    }
//    else {
//        rek(x, distance);
//    }
//}

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
		Matrix moveToSpawnPos;
		moveToSpawnPos.translation(this->spawnPosition);

		Transform = moveToSpawnPos;
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
	//this->glider->BoundingBox.translate(this->Transform.translation());
	this->glider->BoundingBox.transform(this->Transform);


	this->isInWind = false;
	return this->Transform.translation();
}



void Glider::navigateForTesting(float forwardBackward, float UpDown, float LeftRight)
{
	//this->movingVector = Transform.right() * forwardBackward;

}

void Glider::draw(const BaseCamera& Cam)
{
	glider->draw(Cam);
}




float velocity = 5;
//float lift = 0.000111788491;
float lift = 0.1;

float drag = 0.0000324;
float weight = 0.9317522032;

float pitch = 0;

float x = 0;
float tmp = 0;

//Bug: wenn omega ungef�hr gleich ist wie phi
//https://www.youtube.com/watch?v=uIgEwJVWVpY&t=368s&ab_channel=ChristopherScottVaughen
void Glider::calcNextMovment()
{
	float distance = 0;


	if (this->currentWind != nullptr && this->currentWind->isActiv == false) {
		 distance = (this->transform().translation().Y - this->currentWind->transform().translation().Y);
		 lift = 20000 / (distance * distance);
		 //std::cout << lift << std::endl;
		 this->currentWind->trigger();
	}
	else {
		lift = 0.1;
	}
	
	
	float phi = acos(Transform.forward().normalize().dot(unitVecPosZ));

	if (Transform.forward().normalize().Y < 0)
	{
		phi *= -1;
	}

	float omega = acos(Transform.right().normalize().dot(unitVecPosX));
	float angle = 0;
	//angle = std::min((float)(M_PI / 2), omega);
	angle = atan2f(Transform.right().normalize().Y, Transform.right().normalize().X);

	//if (Transform.right().normalize().Y < 0)
	//{

	//	angle *= -1;

	//}

	std::cout << angle << std::endl;





	if (phi > 0) {
		velocity = velocity - phi * std::abs(phi) / 100;

	}
	else {
		velocity = velocity - phi * std::abs(phi) / 5;
	}
	if (velocity < -1) {
		velocity = -1;
	}
	if (velocity > 15) {
		velocity = 15;
	}

	if (phi < -M_PI / 2 && phi > M_PI / 2) {
		pitch = 0;
	}
	else {
		pitch = 1 / ((abs(velocity) + 1) * 5);
	}

	//std::cout << omega << std::endl;


	//std::cout << velocity << "|" << pitch <<  "|" << phi << "|" << (omega * abs(omega)) << std::endl;

	float rotUpDownKoe = (1 + (abs(omega))) / 6; //je gr��er abs(omega) desto kleiner



	this->nextPos = Transform.forward() * velocity + Transform.backward() * drag + unitVecPosY * lift + unitVecNegY * weight;
	this->nextRotX = (this->rotUpDown * rotUpDownKoe) + pitch;

	//this->nextRotZ = (this->rotLeftRight - (omega* abs(omega)));
	this->nextRotZ = (this->rotLeftRight - (angle *abs(angle) / 4));


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

	drag = 0.0000324;
	weight = 0.9317522032;

	this->glider->BoundingBox.translate(spawnPosition);
}


