// Otto.h

#ifndef __OTTO__
#define __OTTO__
#include <SDL.h>
#include "SDL2Berzerk.h"
#include "AnimateObj.h"
#include "Player.h"

static const int OTTO_WIDTH = 16;
static const int OTTO_HEIGHT = 16;
static const int OTTO_VEL = 5;

class Otto : public AnimateObj
{
public:
	Otto(Texture *, SDL_Color, void(*)(void *), Player *);
	~Otto();

	//void handleEvent(SDL_Event& e);


	// move the robot
	void move();

	void activate(bool x) { isActive = x; }

private:

	Player * player = NULL;

	// velocity of player
	int mVelX, mVelY;

	bool isActive = false;

	double delta_x = 1.0;
	double delta_y = 0.0;
};

#endif
