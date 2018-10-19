// Player.h

#ifndef __PLAYER__
#define __PLAYER__
#include <SDL.h>
#include "SDL2Berzerk.h"
#include "AnimateObj.h"
#include "Texture.h"
#include "Utils.h"

class Player : public AnimateObj
{
	public:
		static const int PLAYER_WIDTH = 16;
		static const int PLAYER_HEIGHT = 34;
		static const int PLAYER_VEL = 2; // 5;

		Player(Texture *, SDL_Color, void (*)(void *));
		~Player();

		// Takes key presses and adjusts the dot's velocity
		void handleEvent(SDL_Event& e);

		// move the player
		void move();

		void reset(SDL_Color, MAZE_EXIT);

		bool IsElectrocuting();


	private:
		//The X and Y offsets of the player
		//int mPosX, mPosY;

		void Fire();

		void GetKeyStates();
		bool JoyStick(SDL_Event &e);

		// velocity of player
		int mVelX, mVelY;

		int playerstate = 0;
		int prevstate = 0;
		int cooldown = 0;
};
#endif
