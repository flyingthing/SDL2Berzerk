// ClockTimer.h

#ifndef __CLOCKTIMER__
#define __CLOCKTIMER__
#include <SDL.h>

// application time based timer
class ClockTimer
{
public:
	//Initializes variables
	ClockTimer();

	//The various clock actions
	void start();
	void stop();
	void pause();
	void unpause();

	//Gets the timer's time
	Uint32 getTicks();

	//Checks the status of the timer
	bool isStarted();
	bool isPaused();

private:
	//The clock time when the timer started
	Uint32 mStartTicks;

	//The ticks stored when the timer was paused
	Uint32 mPausedTicks;

	//The timer status
	bool mPaused;
	bool mStarted;
};

#endif


