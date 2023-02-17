#include "Glider.h"
#define _USE_MATH_DEFINES
#include <math.h>


Vector unitVecNegY = Vector(0, -1, 0);
Vector unitVecPosZ = Vector(0, 0, 1);
Vector unitVecPosX = Vector(1, 0, 0);

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

	

	//float distance = (this->transform().translation().Y - windPos.Y);

	//rek(x, distance);

	//float lift = x ;

	//std::cout << lift << "|" << distance << std::endl;



	//Matrix TM, RT;
	//Vector nextPos = Transform.up() * lift;
	//TM.translation(nextPos * dtime);


	//Rotation
	//float phi = acos(Transform.forward().normalize().dot(unitVecPosZ));
	//float nextRot = -((10 - (phi*phi)) / distance);
	//RT.rotationX(nextRot * dtime);


	//this->Transform = TM * this->Transform;

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
	// Wenn Gleiter gecrasht ist, auf Startposition zurücksetzen
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

//Bug: wenn omega ungefähr gleich ist wie phi
//https://www.youtube.com/watch?v=uIgEwJVWVpY&t=368s&ab_channel=ChristopherScottVaughen
void Glider::calcNextMovment()
{
	float distance = 0;

	if (this->currentWind != nullptr && this->currentWind->isActiv == false) {
		 distance = (this->transform().translation().Y - this->currentWind->transform().translation().Y);
		 lift = 1000 / distance;
	}
	else {
		lift = 0.1;
	}
	

	//float liftKoe = 1;
	if (this->isInWind)
	{
	//	if (x == 0) {
	//		x = 10 * cos(distance * 0.05) + 10;
	//		tmp = distance;
	//	}

	//}
	//if (x != 0) {
	//	x = 10 * cos((tmp) * 0.05) + 10;
	//	tmp = tmp + 0.5;
	//	if (x <= 0.1) {
	//		x = 0;
	//		std::cout << "Heeeee" << std::endl;
	//	}

	}


	//lift = sqrt(x) * 10;

	//std::cout << x << "|" << distance << std::endl;
	



	float phi = acos(Transform.forward().normalize().dot(unitVecPosZ));
	if (Transform.forward().normalize().Y < 0)
	{
		phi *= -1;
	}

	float omega = acos(Transform.right().normalize().dot(unitVecPosX));
	if (Transform.right().normalize().Y < 0)
	{
		omega *= -1;
	}
	if (omega < 0.1 && omega > -0.1) {
		omega = 0;
	}

	if (phi > 0) {
		velocity = velocity - phi * std::abs(phi) / 50;

	}
	else {
		velocity = velocity - phi * std::abs(phi) / 20;
	}
	if (velocity < -1) {
		velocity = -1;
	}
	if (velocity > 12) {
		velocity = 12;
	}

	if (phi < -M_PI / 2 && phi > M_PI / 2) {
		pitch = 0;
	}
	else {
		pitch = 1 / ((abs(velocity) + 1) * 5);
	}


	// std::cout << velocity << "|" << pitch <<  "|" << phi << "|" << omega << std::endl;

	float rotUpDownKoe = (1 + (abs(omega))) / 6; //je größer abs(omega) desto kleiner

	this->nextPos = Transform.forward() * velocity + Transform.backward() * drag + Transform.up() * lift + unitVecNegY * weight;
	this->nextRotX = (this->rotUpDown * rotUpDownKoe) + pitch;
	this->nextRotZ = (this->rotLeftRight) - ((omega * abs(omega)) / 4);

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


