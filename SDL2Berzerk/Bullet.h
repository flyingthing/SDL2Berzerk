// Bullet.h

#ifndef __BULLET__
#define __BULLET__
#include <SDL.h>
#include "SDL2Berzerk.h"
#include "AnimateObj.h"
#include "Utils.h"

class Bullet : public AnimateObj
{
public:
	static const int BULLET_VEL = 5;

	Bullet(Texture *, SDL_Color, HEADING);
	~Bullet();

	//void handleEvent(SDL_Event& e);

	// move the robot
	void move();
	int xOffSet();

	bool deltag = false;

private:

	// velocity of bullet
	int mVelX, mVelY;
};

#endif

