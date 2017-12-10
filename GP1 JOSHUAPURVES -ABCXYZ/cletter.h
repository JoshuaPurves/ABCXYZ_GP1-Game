/*
=================
cLetter.h
- Header file for class definition - SPECIFICATION
- Header file for the pencil class which is a child of cSprite class
=================
*/
#ifndef _CLETTER_H
#define _CLETTER_H
#include "cSprite.h"

class cLetter : public cSprite
{
private:
	SDL_Point LetterVelocity;

public:
	cLetter();
	void update(double deltaTime);		
	void setLetterVelocity(SDL_Point letterVel);   
	SDL_Point getletterVelocity();				
};
#endif