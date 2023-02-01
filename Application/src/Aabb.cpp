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
    
}
AABB::AABB(const Vector& min, const Vector& max) : Min(min), Max(max)
{
    
}
AABB::AABB(float minX, float minY, float minZ, float maxX, float maxY, float maxZ) : Min(minX, minY, minZ), Max(maxX, maxY, maxZ)
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

/// <summary>
///     Bewegt die BoundingBox zu einer bestimmten Position
/// </summary>
void AABB::translate(Vector position)
{
    Min += position;
    Max += position;
}
