/*
=================
cAsteroid.cpp
- Header file for class definition - IMPLEMENTATION
=================
*/
#include "cLetter.h"

/*
=================================================================
Defualt Constructor
=================================================================
*/
cLetter::cLetter() : cSprite()
{
	this->LetterVelocity = { 0, 0 };
}
/*
=================================================================
Update the sprite position
=================================================================
*/

void cLetter::update(double deltaTime)
{

	this->setSpriteRotAngle(this->getSpriteRotAngle() +(5.0f * deltaTime)); 
	if (this->getSpriteRotAngle() > 360)
	{
		this->setSpriteRotAngle(this->getSpriteRotAngle() -360);
	}

	SDL_Rect currentSpritePos = this->getSpritePos();
	currentSpritePos.x += this->getSpriteTranslation().x * deltaTime;
	currentSpritePos.y += this->getSpriteTranslation().y * deltaTime;

	this->setSpritePos({ currentSpritePos.x, currentSpritePos.y });
	cout << "Letter position - x: " << this->getSpritePos().x << " y: " << this->getSpritePos().y << " deltaTime: " << deltaTime << endl;
	this->setBoundingRect(this->getSpritePos());
}
/*
=================================================================
  Sets the velocity for the Asteroid
=================================================================
*/
void cLetter::setLetterVelocity(SDL_Point LetterVel)
{
	LetterVelocity = LetterVel;
}
/*
=================================================================
  Gets the Asteroid velocity
=================================================================
*/
SDL_Point cLetter::getletterVelocity()
{
	return LetterVelocity;
}
