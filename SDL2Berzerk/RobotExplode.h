// RobotExplode.h

#ifndef __ROBOTEXPLODE__
#define __ROBOTEXPLODE__
#include "Robot.h"
#include <SDL.h>
#include "SDL2Berzerk.h"
#include "AnimateObj.h"
#include "Texture.h"

class RobotExplode : public AnimateObj
{
public:
	static const int ROBOTEXPLODE_WIDTH = 32;
	static const int ROBOTEXPLODE_HEIGHT = 32;
	static const int ROBOTEXPLODE_VEL = 0;

	RobotExplode(Texture *, void (*)(void *), Robot *);
	~RobotExplode();

	void handleEvent(SDL_Event& e);


private:

	// velocity of player
	int mVelX, mVelY;
};
#endif

