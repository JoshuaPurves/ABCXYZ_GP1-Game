/*
=================
cBall.h
- Header file for class definition - SPECIFICATION
- Header file for the Ball class which is a child of cSprite class
=================
*/
#ifndef _CBALL_H
#define _CBALL_H
#include "cSprite.h"


class cBall : public cSprite
{
private:
	SDL_Point ballVelocity;

public:
	cBall();
	void update(double deltaTime);		// Bullet update method
	void setBallVelocity(SDL_Point ballVel);   // Sets the velocity for the Bullet
	SDL_Point getBallVelocity();				 // Gets the Bullet velocity
};
#endif