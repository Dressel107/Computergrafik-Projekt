//
//  LineBoxModel.cpp
//  CGXcode
//
//  Created by Philipp Lensing on 10.10.16.
//  Copyright © 2016 Philipp Lensing. All rights reserved.
//

#include "LineBoxModel.h"

LineBoxModel::LineBoxModel( float Width, float Height, float Depth )
{
	// TODO: Add your code (Exercise 1)   

	VB.begin();

	float beginX = 0 - Width / 2;
	float beginY =  Height / 2;
	float beginZ = 0 - Depth / 2;

	VB.addVertex(beginX, beginY, beginZ);
	VB.addVertex(beginX + Width, beginY, beginZ);

	VB.addVertex(beginX + Width, beginY, beginZ);
	VB.addVertex(beginX + Width, -beginY, beginZ);
	
	VB.addVertex(beginX + Width, -beginY, beginZ);
	VB.addVertex(beginX, -beginY, beginZ);
	
	VB.addVertex(beginX, -beginY, beginZ);
	VB.addVertex(beginX, beginY, beginZ);

	VB.addVertex(beginX, beginY, beginZ);
	VB.addVertex(beginX, beginY, beginZ+ Depth);

	VB.addVertex(beginX, beginY, beginZ + Depth);
	VB.addVertex(beginX, -beginY, beginZ + Depth);

	VB.addVertex(beginX, -beginY, beginZ + Depth);
	VB.addVertex(beginX, -beginY, beginZ);

	VB.addVertex(beginX, -beginY, beginZ + Depth);
	VB.addVertex(beginX + Width, -beginY, beginZ + Depth);

	VB.addVertex(beginX + Width, -beginY, beginZ + Depth);
	VB.addVertex(beginX + Width, beginY, beginZ + Depth);

	VB.addVertex(beginX + Width, beginY, beginZ + Depth);
	VB.addVertex(beginX, beginY, beginZ + Depth);

	VB.addVertex(beginX + Width, beginY, beginZ + Depth);
	VB.addVertex(beginX + Width, beginY, beginZ);

	VB.addVertex(beginX + Width, -beginY, beginZ + Depth);
	VB.addVertex(beginX + Width, -beginY, beginZ);







	

	VB.end();
}

void LineBoxModel::draw(const BaseCamera& Cam)
{
	BaseModel::draw(Cam);

	VB.activate();

	glDrawArrays(GL_LINES, 0, VB.vertexCount());

	VB.deactivate();
}
