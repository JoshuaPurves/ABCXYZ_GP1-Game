/*
=================
cBullet.cpp
- Header file for class definition - IMPLEMENTATION
=================
*/
#include "cBall.h"

/*
=================================================================
Defualt Constructor
=================================================================
*/
cBall::cBall() : cSprite()
{
	this->ballVelocity = { 1, 1 };
}
/*
=================================================================
Update the sprite position
=================================================================
*/

void cBall::update(double deltaTime)
{

	FPoint direction = { 0.0, 0.0};
	direction.X = (sin((this->getSpriteRotAngle())*PI/180));
	direction.Y = -(cos((this->getSpriteRotAngle())*PI / 180));

	direction.X *= this->getSpriteTranslation().x;
	direction.Y *= this->getSpriteTranslation().y;

	this->ballVelocity.x = this->ballVelocity.x + direction.X;
	this->ballVelocity.y = this->ballVelocity.y + direction.Y;

	SDL_Rect currentSpritePos = this->getSpritePos();
	currentSpritePos.x += this->ballVelocity.x * deltaTime;
	currentSpritePos.y += this->ballVelocity.y * deltaTime;

	this->setSpritePos({ currentSpritePos.x, currentSpritePos.y });

	cout << "Ball position - x: " << this->getSpritePos().x << " y: " << this->getSpritePos().y << " deltaTime: " << deltaTime << endl;

	this->setBoundingRect(this->getSpritePos());

}
/*
=================================================================
  Sets the velocity for the Bullet
=================================================================
*/
void cBall::setBallVelocity(SDL_Point ballVel)
{
	this->ballVelocity = ballVel;
}
/*
=================================================================
  Gets the Bullet velocity
=================================================================
*/
SDL_Point cBall::getBallVelocity()
{
	return this->ballVelocity;
}
