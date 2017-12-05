/*
=================
cPencil.cpp
- Header file for class definition - IMPLEMENTATION
=================
*/
#include "cPencil.h"

/*
=================================================================
Defualt Constructor
=================================================================
*/
cPencil::cPencil() : cSprite()
{
	this->PencilVelocity = { 0, 0 };
	FPoint direction = { 0, 0 };
}
/*
=================================================================
Update the sprite position
=================================================================
*/

void cPencil::update(double deltaTime)
{
	direction.X = (sin(this->getSpriteRotAngle()*PI / 180));
	direction.Y = -(cos(this->getSpriteRotAngle()*PI / 180));

	direction.X *= this->getSpriteTranslation().x;
	direction.Y *= this->getSpriteTranslation().y;

	this->PencilVelocity.x = this->PencilVelocity.x + direction.X;
	this->PencilVelocity.y = this->PencilVelocity.y + direction.Y;

	SDL_Rect currentSpritePos = this->getSpritePos();
	currentSpritePos.x += this->PencilVelocity.x * deltaTime;
	currentSpritePos.y += this->PencilVelocity.y * deltaTime;

	this->PencilVelocity.x *= 0.95;
	this->PencilVelocity.y *= 0.95;

	this->setSpritePos({ currentSpritePos.x , currentSpritePos.y });
	this->setBoundingRect(this->getSpritePos());
}
/*
=================================================================
Sets the velocity for the rocket
=================================================================
*/
void cPencil::setPencilVelocity(SDL_Point pencilVel)
{
	PencilVelocity = pencilVel;
}
/*
=================================================================
Gets the rocket velocity
=================================================================
*/
SDL_Point cPencil::getPencilVelocity()
{
	return PencilVelocity;
}


