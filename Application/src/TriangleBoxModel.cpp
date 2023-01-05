//
//  TriangleBoxModel.cpp
//  CGXcode
//
//  Created by Philipp Lensing on 10.10.16.
//  Copyright © 2016 Philipp Lensing. All rights reserved.
//

#include "TriangleBoxModel.h"

TriangleBoxModel::TriangleBoxModel(float Width, float Height, float Depth)
{
	// Eckpunkte/Vertices
	Vector untenLinksVorne = Vector(-Width/2, -Height/2, Depth/2);
	Vector untenRechtsVorne = Vector(Width/2 , -Height / 2, Depth/2);
	Vector obenRechtsVorne = Vector(Width/2, Height/2, Depth/2);
	Vector obenLinksVorne = Vector(-Width/2, Height/2, Depth/2);

	Vector untenLinksHinten = Vector(-Width/2, -Height/2, -Depth/2);
	Vector untenRechtsHinten = Vector(Width/2, -Height/2, -Depth/2);
	Vector obenRechtsHinten = Vector(Width/2, Height/2, -Depth/2);
	Vector obenLinksHinten = Vector(-Width/2, Height/2, -Depth/2);


	// Normale aus Eckpunkten berechnen:
	// (A - C) x (B - C)

	VB.begin();

	// Vorderseite
	Vector n = (obenLinksVorne - obenRechtsVorne).cross(untenLinksVorne - obenRechtsVorne);

	VB.addNormal(n); VB.addTexcoord0(0, 0.5);	VB.addVertex(obenLinksVorne); // 0
	VB.addNormal(n); VB.addTexcoord0(0, 1);	VB.addVertex(untenLinksVorne); // 1
	VB.addNormal(n); VB.addTexcoord0(1, 0.5);	VB.addVertex(obenRechtsVorne); // 2
	VB.addNormal(n); VB.addTexcoord0(1, 1);	VB.addVertex(untenRechtsVorne); // 3

	// Rückseite
	n = (obenRechtsHinten - obenLinksHinten).cross(untenRechtsHinten - obenLinksHinten);

	VB.addNormal(n); VB.addTexcoord0(0, 0);	VB.addVertex(obenRechtsHinten); // 4
	VB.addNormal(n); VB.addTexcoord0(0, 1);	VB.addVertex(untenRechtsHinten); // 5
	VB.addNormal(n); VB.addTexcoord0(1, 0);	VB.addVertex(obenLinksHinten); // 6
	VB.addNormal(n); VB.addTexcoord0(1, 1);	VB.addVertex(untenLinksHinten); // 7

	// Rechte Seite
	n = (obenRechtsVorne - obenRechtsHinten).cross(untenRechtsVorne - obenRechtsHinten);

	VB.addNormal(n); VB.addTexcoord0(0, 0);	VB.addVertex(obenRechtsVorne); // 8
	VB.addNormal(n); VB.addTexcoord0(0, 1);	VB.addVertex(untenRechtsVorne); // 9
	VB.addNormal(n); VB.addTexcoord0(1, 0);	VB.addVertex(obenRechtsHinten); // 10
	VB.addNormal(n); VB.addTexcoord0(1, 1);	VB.addVertex(untenRechtsHinten); // 11

	// Linke Seite
	n = (obenLinksHinten - obenLinksVorne).cross(untenLinksHinten - obenLinksVorne);

	VB.addNormal(n); VB.addTexcoord0(0, 0);	VB.addVertex(obenLinksHinten); // 12
	VB.addNormal(n); VB.addTexcoord0(0, 1);	VB.addVertex(untenLinksHinten); // 13
	VB.addNormal(n); VB.addTexcoord0(1, 0);	VB.addVertex(obenLinksVorne); // 14
	VB.addNormal(n); VB.addTexcoord0(1, 1);	VB.addVertex(untenLinksVorne); // 15

	// Oben
	n = (obenLinksHinten - obenRechtsHinten).cross(obenLinksVorne - obenRechtsHinten);

	VB.addNormal(n); VB.addTexcoord0(0, 0);	VB.addVertex(obenLinksHinten); // 16
	VB.addNormal(n); VB.addTexcoord0(0, 1);	VB.addVertex(obenLinksVorne); // 17
	VB.addNormal(n); VB.addTexcoord0(1, 0);	VB.addVertex(obenRechtsHinten); // 18
	VB.addNormal(n); VB.addTexcoord0(1, 1);	VB.addVertex(obenRechtsVorne); // 19

	// Unten
	n = (untenLinksVorne - untenRechtsVorne).cross(untenLinksHinten - untenRechtsHinten);

	VB.addNormal(n); VB.addTexcoord0(1, 1);	VB.addVertex(untenRechtsHinten); // 20
	VB.addNormal(n); VB.addTexcoord0(1, 0);	VB.addVertex(untenRechtsVorne); // 21
	VB.addNormal(n); VB.addTexcoord0(0, 1);	VB.addVertex(untenLinksHinten); // 22
	VB.addNormal(n); VB.addTexcoord0(0, 0);	VB.addVertex(untenLinksVorne); // 23

	VB.end();

	// Vertices GEGEN den Uhrzeigesinn hinzufügen, damit das Dreieck sichtbar ist,
	// wenn die Kamera frontal dieses schaut
	IB.begin();

	// Vorderseite
	IB.addIndex(0); IB.addIndex(1); IB.addIndex(2);
	IB.addIndex(1); IB.addIndex(3); IB.addIndex(2);

	// Rückseite
	IB.addIndex(4); IB.addIndex(5); IB.addIndex(6);
	IB.addIndex(5); IB.addIndex(7); IB.addIndex(6);

	// Rechte Seite
	// IB.addIndex(3); IB.addIndex(4); IB.addIndex(2);
	IB.addIndex(8); IB.addIndex(9); IB.addIndex(10);
	IB.addIndex(9); IB.addIndex(11); IB.addIndex(10);

	// Linke Seite
	IB.addIndex(12); IB.addIndex(13); IB.addIndex(14);
	IB.addIndex(13); IB.addIndex(15); IB.addIndex(14);

	// Oben
	IB.addIndex(16); IB.addIndex(17); IB.addIndex(18);
	IB.addIndex(17); IB.addIndex(19); IB.addIndex(18);

	// Unten
	IB.addIndex(20); IB.addIndex(21); IB.addIndex(22);
	IB.addIndex(21); IB.addIndex(23); IB.addIndex(22);

	IB.end();
}

void TriangleBoxModel::draw(const BaseCamera& Cam)
{
	BaseModel::draw(Cam);

	VB.activate();
	IB.activate();

	glDrawElements(GL_TRIANGLES, IB.indexCount(), IB.indexFormat(), 0);

	IB.deactivate();
	VB.deactivate();
}
