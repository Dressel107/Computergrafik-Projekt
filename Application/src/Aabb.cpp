//
//  Aabb.cpp
//  CGXcode
//
//  Created by Philipp Lensing on 02.11.16.
//  Copyright © 2016 Philipp Lensing. All rights reserved.
//

#include "Aabb.h"

AABB::AABB()
{
    Min = Vector(0, 0, 0);
    Max = Vector(0, 0, 0);
    SpawnMin = Vector(0, 0, 0);
    SpawnMax = Vector(0, 0, 0);
}
AABB::AABB(const Vector& min, const Vector& max) : Min(min), Max(max), SpawnMin(min), SpawnMax(max)
{
    
}
AABB::AABB(float minX, float minY, float minZ, float maxX, float maxY, float maxZ) : Min(minX, minY, minZ), Max(maxX, maxY, maxZ), SpawnMin(minX, minY, minZ), SpawnMax(maxX, maxY, maxZ)
{
    
}

Vector AABB::size() const
{
    return Max-Min;
}

/// <summary>
///     Prüft, ob der Vektor innerhalb der Box liegt.
/// </summary>
bool AABB::intersectWith(Vector v)
{
    if (v.X >= Min.X && v.X <= Max.X && v.Y >= Min.Y && v.Y <= Max.Y && v.Z >= Min.Z && v.Z <= Max.Z)
    {
        return true;
    }

    return false;
}

/// <summary>
///     Prüft, ob sich die übergebene Box mit der eigenen schneidet.
/// </summary>
bool AABB::intersectWith(AABB aabb)
{
    if (Min.X <= aabb.Max.X && Max.X >= aabb.Min.X && Min.Y <= aabb.Max.Y && Max.Y >= aabb.Min.Y && Min.Z <= aabb.Max.Z && Max.Z >= aabb.Min.Z)
    {
        return true;
    }

    return false;
}

void AABB::transform(Matrix matrix)
{
    Min = matrix * SpawnMin;
    Max = matrix * SpawnMax;

    // Prüfen, ob Min und Max richtig angeordnet sind
    if (Min.X > Max.X)
    {
        float temp = Min.X;
        Min.X = Max.X;
        Max.X = temp;
    }

    if (Min.Y > Max.Y)
    {
        float temp = Min.Y;
        Min.Y = Max.Y;
        Max.Y = temp;
    }

    if (Min.Z > Max.Z)
    {
        float temp = Min.Z;
        Min.Z = Max.Z;
        Max.Z = temp;
    }
}

