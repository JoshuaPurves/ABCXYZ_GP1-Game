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
	FPoint direction;

public:
	cPencil();
	void update(double deltaTime);		// pencil update method
	void setPencilVelocity(SDL_Point pencilVel);   // Sets the velocity for the pencil
	SDL_Point getPencilVelocity();				 // Gets the pencil velocity
};
#endif