/*
=================
cRocket.h
- Header file for class definition - SPECIFICATION
- Header file for the Rocket class which is a child of cSprite class
=================
*/
#ifndef _CPENCIL_H
#define _CPENCIL_H
#include "cSprite.h"

class cPencil : public cSprite
{
private:
	SDL_Point PencilVelocity;

public:
	cPencil();
	void update(double deltaTime);		// Rocket update method
	void setPencilVelocity(SDL_Point rocketVel);   // Sets the velocity for the rocket
	SDL_Point getPencilVelocity();				 // Gets the rocket velocity
};
#endif