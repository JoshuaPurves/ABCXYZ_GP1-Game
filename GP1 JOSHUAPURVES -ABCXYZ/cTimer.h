#ifndef  _CTIMER_H
#define _CTIMER_H

//Ather Omar: SDL 2.0 Tutorials

class Timer {
private:
	static Timer* sInstance;

	unsigned int StartTicks;
	unsigned int ElapsedTicks;
	float theDeltaTime;
	float theTimeScale;

public:
	static Timer* Instance();
	static void Release();

	void Reset();
	float DeltaTime();

	void TimeScale(float t);
	float TimeScale();

	void update();

private:
	Timer();
	~Timer();

};
#endif 
