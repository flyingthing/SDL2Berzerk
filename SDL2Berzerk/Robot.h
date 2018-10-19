// Robot.h

#ifndef __ROBOT__
#define __ROBOT__
#include <SDL.h>
#include "SDL2Berzerk.h"
#include "AnimateObj.h"
#include "Texture.h"

static const int ROBOT_WIDTH = 16;
static const int ROBOT_HEIGHT = 22;
static const int ROBOT_VEL = 5;

static int robotcount = 0;

class Robot : public AnimateObj
{
public:
	Robot(Texture *, SDL_Color, void (*)(void *));
	~Robot();

	static int objectCount;

	// move the robot
	void move(int, int);


	void activate(bool x) { isActive = x; }

	bool deltag = false;

private:

	// velocity of player
	int mVelX, mVelY;

	bool isActive = false;
};

#endif
