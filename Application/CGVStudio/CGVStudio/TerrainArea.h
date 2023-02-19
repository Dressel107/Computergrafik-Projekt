#pragma once
#include "../../src/vector.h"
#include "../../src/Aabb.h"

class TerrainArea
{
public:
	TerrainArea();
	Vector* vertices;
	AABB boundingbox;
};

