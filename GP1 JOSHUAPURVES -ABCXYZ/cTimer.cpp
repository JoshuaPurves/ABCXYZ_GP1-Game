#include "cTimer.h"
#include "abcxyzGame.h"

Timer* Timer::sInstance = NULL;

Timer* Timer::Instance()
{
	if (sInstance == NULL)
		sInstance = new Timer();
	return sInstance;
}

void Timer::Release()
{
	delete sInstance;
	sInstance = NULL;
}

Timer::Timer()
{
	Reset();
	theTimeScale = 1.0f;
}

Timer::~Timer()
{

}

void Timer::Reset() {
	StartTicks = SDL_GetTicks();
	ElapsedTicks = 0;
	theDeltaTime = 0.0f;
}

float Timer::DeltaTime()
{
	return theDeltaTime;
}

void Timer::TimeScale(float t)
{
	theTimeScale = t;
}

float Timer::TimeScale()
{
	return theTimeScale;

}

void Timer::update()
{
	ElapsedTicks = SDL_GetTicks() - StartTicks;
	theDeltaTime = ElapsedTicks * 0.001f;
}