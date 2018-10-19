#include "Player.h"
#include <iostream>
#include <string.h>
#include <cmath>

struct MovDict
{
	HEADING fire;
	int x, y;
	std::string direction;
};

// LEFT  0x01
// RIGHT 0x02
// UP    0x04
// DOWN  0x08
// SPACE 0x10

//Analog joystick dead zone
const int JOYSTICK_DEAD_ZONE = 8000;


enum  KEYBRD { LEFT_KEY = 0x01, RIGHT_KEY = 0x02, UP_KEY = 0x04, DOWN_KEY = 0x08, SPACE_KEY = 0x10 };

static MovDict playermovs[] = {  { HEADING::_NONE,0,0,"still"},
								 { HEADING::_NONE,-Player::PLAYER_VEL,0,"W"},							// LEFT
								 { HEADING::_NONE, Player::PLAYER_VEL,0,"E"},							// RIGHT
								 { HEADING::_NONE,0,0,"still"},											// LEFT/RIGHT CANCEL
								 { HEADING::_NONE,0,-Player::PLAYER_VEL,"N"},							// UP
								 { HEADING::_NONE,-Player::PLAYER_VEL,-Player::PLAYER_VEL,"W"},			// UP & LEFT
								 { HEADING::_NONE,Player::PLAYER_VEL,-Player::PLAYER_VEL,"E"},			// UP & RIGHT
								 { HEADING::_NONE,0,-Player::PLAYER_VEL,"N"},							// UP LEFT/RIGHT CANCEL
								 { HEADING::_NONE,0,Player::PLAYER_VEL,"S"},							// DOWN
								 { HEADING::_NONE,-Player::PLAYER_VEL,Player::PLAYER_VEL,"W"},			// DOWN & LEFT
								 { HEADING::_NONE,Player::PLAYER_VEL,Player::PLAYER_VEL,"E"},			// DOWN & RIGHT
								 { HEADING::_NONE,0,Player::PLAYER_VEL,"S"},							// DOWN LEFT/RIGHT CANCEL
								 { HEADING::_NONE,0,0,"still"},											// UP/DOWN CANCEL
								 { HEADING::_NONE,-Player::PLAYER_VEL,0,"W"},							// LEFT UP/DOWN CANCEL
								 { HEADING::_NONE,Player::PLAYER_VEL,0,"E"},							// RIGHT UP/DOWN CANCEL
								 { HEADING::_NONE,0,0,"still"},											// UP/DOWN LEFT/RIGHT CANCEL
								 { HEADING::_NONE,0,0,"still"},											// SHOOT NO-ARROW
								 { HEADING::W,0,0,"WW"},											// SHOOT-LEFT
								 { HEADING::E,0,0,"EE"},											// SHOOT-RIGHT
								 { HEADING::_NONE,0,0,"still"},
								 { HEADING::N,0,0,"EN"},											// SHOOT UP
								 { HEADING::NW,0,0,"WNW"},											// SHOOT UP/LEFT
								 { HEADING::NE,0,0,"ENE"},											// SHOOT UP/RIGHT
								 { HEADING::N,0,0,"EN"},											// FACE??? SHOOT UP
								 { HEADING::S,0,0,"ES"},											// FACE??? SHOOT DOWN
								 { HEADING::SW,0,0,"WSW"},											// FACE-LEFT SHOOT DOWN/LEFT
								 { HEADING::SE,0,0,"ESE"},											// FACE-RIGHT SHOOT DOWN/RIGHT
								 { HEADING::S,0,0,"ES"},											// FACE??? SHOOT DOWN
								 { HEADING::_NONE,0,0,"still"},
								 { HEADING::W,0,0,"WW"},											// FACE-LEFT SHOOT LEFT
								 { HEADING::E,0,0,"EE"},											// FACE-RIGHT SHOOT RIGHT
								 { HEADING::_NONE,0,0,"still"}
};

Player::Player(Texture *pTexture, SDL_Color color, void(*cb)(void *)) : AnimateObj(OBJ_TYPE::PLAYER, pTexture, color, PLAYER_WIDTH, PLAYER_HEIGHT, cb)
{
	//Initialize the offsets
	//mPosX = 40;
	//mPosY = 40;

	//Initialize the velocity
	mVelX = 0;
	mVelY = 0;

	addPattern("still", { 0 });

	// running
	//addPattern("E", { 1,2,3,2 });
	addPattern("E", { 1,3,2,3,1,2,3,1,3,2 });
	addPattern("W", { 6,5,4,5 });
	addPattern("N", { 1,2,3,2 });
	addPattern("S", { 1,2,3,2 });

	// shooting
	addPattern("EE", { 12 });         // facing E shooting E
	addPattern("EN", { 13 });         // facing E shooting N
	addPattern("ES", { 14 });         // facing E shooting S
	addPattern("ESE", { 15 });        // facing E shooting SE
	addPattern("ENE", { 16 });        // facing E shooting NE
	addPattern("WW", { 17 });         // facing W shooting W
	addPattern("WN", { 18 });         // facing W shooting N
	addPattern("WS", { 19 });         // facing W shooting S
	addPattern("WSW", { 20 });        // facing W shooting SW
	addPattern("WNW", { 21 });        // facing W shooting NW

	addPattern("electrocuting", { 8,9,10,11 });

	reset(color, MAZE_EXIT::LEFT);
}

Player::~Player()
{
}



bool Player::IsElectrocuting()
{
	return (currentPattern.compare("electrocuting") == 0) ? true : false;
}

void Player::handleEvent(SDL_Event& e)
{
	if (IsElectrocuting() == true)
	{
		return;
	}

	std::string direction = "still";
	int dir = HEADING::_NONE;

	//JoyStick(e);
	GetKeyStates();

	if (playerstate != prevstate)
	{
		setPattern(playermovs[playerstate].direction);
		prevstate = playerstate;
	}

	mVelX = playermovs[playerstate].x;
	mVelY = playermovs[playerstate].y;

	if (playerstate & KEYBRD::SPACE_KEY)
	{
		int tmp = SDL_GetTicks();

		if (tmp - cooldown >= 300)
		{
			//printf("FIRE\n");
			Fire();
			cooldown = tmp;
		}
	}

	//printf("TICKS: %0d\n", SDL_GetTicks());

	//printf("%d\n", e.key.keysym.sym);

	//If a key was pressed
	/*
	if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
	{
		//Adjust the velocity
		switch (e.key.keysym.sym)
		{
		case SDLK_UP: playerstate |= PLAYERKEY::UP_KEY; break;
		case SDLK_DOWN: playerstate |= PLAYERKEY::DOWN_KEY; break;
		case SDLK_LEFT: playerstate |= PLAYERKEY::LEFT_KEY; break;
		case SDLK_RIGHT: playerstate |= PLAYERKEY::RIGHT_KEY; break;
		case SDLK_SPACE: playerstate |= PLAYERKEY::FIRE;
			dir = firedirection();
			UserEvent(USEREVENTS::PLAYER_BULLET_FIRE, (void *)dir, NULL);
			break;
		default:
			break;
		}

		//setPattern(direction);
		//mVelX += playermovs[playerstate].x * (-1)
		//mVelY += playermovs[playerstate].y;
		setPattern(playermovs[playerstate].direction);
		mVelX = playermovs[playerstate].x;
		mVelY = playermovs[playerstate].y;
	}
	//If a key was released
	else if (e.type == SDL_KEYUP && e.key.repeat == 0)
	{
		//setPattern("still");

		//Adjust the velocity
		switch (e.key.keysym.sym)
		{
		case SDLK_UP: playerstate &= ~PLAYERKEY::UP_KEY; break;
		case SDLK_DOWN: playerstate &= ~PLAYERKEY::DOWN_KEY; break;
		case SDLK_LEFT: playerstate &= ~PLAYERKEY::LEFT_KEY; break;
		case SDLK_RIGHT: playerstate &= ~PLAYERKEY::RIGHT_KEY; break;
		case SDLK_SPACE: playerstate &= ~PLAYERKEY::FIRE; break;
		}

		mVelX = playermovs[playerstate].x;
		mVelY = playermovs[playerstate].y;
		setPattern(playermovs[playerstate].direction);
	}
	*/

	//printf("%d\n", playerstate);
	//std::cout << getPattern() << "\n";
	//printf("%0x %d %d\n", playerstate, mVelX, mVelY);
}

bool Player::JoyStick(SDL_Event& e)
{
	//Normalized direction
	int xDir = 0;
	int yDir = 0;

	//Motion on controller 0
	if (e.jaxis.which == 0)
	{
		//X axis motion
		if (e.jaxis.axis == 0)
		{
			//Left of dead zone
			if (e.jaxis.value < -JOYSTICK_DEAD_ZONE)
			{
				playerstate |= KEYBRD::LEFT_KEY;
				xDir = -1;
			}
			//Right of dead zone
			else if (e.jaxis.value > JOYSTICK_DEAD_ZONE)
			{
				playerstate |= KEYBRD::RIGHT_KEY;
				xDir = 1;
			}
			else
			{
				playerstate &= ~(KEYBRD::LEFT_KEY | KEYBRD::RIGHT_KEY);
				xDir = 0;
			}
		}
		//Y axis motion
		else if (e.jaxis.axis == 1)
		{
			//Below of dead zone
			if (e.jaxis.value < -JOYSTICK_DEAD_ZONE)
			{
				playerstate |= KEYBRD::UP_KEY;
				yDir = -1;
			}
			//Above of dead zone
			else if (e.jaxis.value > JOYSTICK_DEAD_ZONE)
			{
				playerstate |= KEYBRD::DOWN_KEY;
				xDir = 1;
			}
			else
			{
				playerstate &= ~(KEYBRD::UP_KEY | KEYBRD::DOWN_KEY);
				yDir = 0;
			}
		}

		//Calculate angle
		double joystickAngle = atan2((double)yDir, (double)xDir) * (180.0 / M_PI);

		//Correct angle
		if (xDir == 0 && yDir == 0)
		{
			joystickAngle = 0;
		}
		else
		{
			//printf("JOYSTICK %f\n", joystickAngle);
		}
	}


	int numButtons = SDL_JoystickNumButtons(gGameController);

	if (e.type == SDL_JOYBUTTONDOWN)
	{
		playerstate |= KEYBRD::SPACE_KEY;
	}
	
	if (e.type == SDL_JOYBUTTONUP)
	{
		playerstate &= ~KEYBRD::SPACE_KEY;
	}

	//if (SDL_JoystickGetButton)


	return true;
}

void Player::GetKeyStates()
{
	const Uint8 *keys = SDL_GetKeyboardState(NULL);

	if (keys[SDL_SCANCODE_UP])
		playerstate |= KEYBRD::UP_KEY;
	else
		playerstate &= ~KEYBRD::UP_KEY;

	if (keys[SDL_SCANCODE_DOWN])
		playerstate |= KEYBRD::DOWN_KEY;
	else
		playerstate &= ~KEYBRD::DOWN_KEY;

	if (keys[SDL_SCANCODE_LEFT])
		playerstate |= KEYBRD::LEFT_KEY;
	else
		playerstate &= ~KEYBRD::LEFT_KEY;

	if (keys[SDL_SCANCODE_RIGHT])
		playerstate |= KEYBRD::RIGHT_KEY;
	else
		playerstate &= ~KEYBRD::RIGHT_KEY;

	if (keys[SDL_SCANCODE_SPACE])
		playerstate |= KEYBRD::SPACE_KEY;
	else
		playerstate &= ~KEYBRD::SPACE_KEY;

	// if player fire
	/*
	if (playerstate & KEYBRD::SPACE_KEY)
	{
		int tmp = SDL_GetTicks();

		if (tmp - cooldown >= 300)
		{
			//printf("FIRE\n");
			Fire();
			cooldown = tmp;
		}
	}
	*/

	//printf("%x\n", playerstate);

}

void Player::Fire()
{
	int dir = (int)playermovs[playerstate].fire;
	UserEvent(USEREVENTS::PLAYER_BULLET_FIRE, (void *)dir, NULL);
}

void Player::reset(SDL_Color color, MAZE_EXIT exit)
{
	// init the velocity
	mVelX = 0;
	mVelY = 0;
	setPattern("still");

	setColor(color);

	int x = 0, y = 0;

	switch (exit)
	{
		case MAZE_EXIT::TOP:
			x = MAZE_XCENTER - PLAYER_WIDTH / 2;
			y = MAZE_YMIN;
			break;

		case MAZE_EXIT::BOTTOM:
			x = MAZE_XCENTER - PLAYER_WIDTH / 2;
			y = MAZE_YMAX - PLAYER_HEIGHT;
			break;

		case MAZE_EXIT::LEFT:
			x = MAZE_XMIN;
			y = MAZE_YCENTER - Player::PLAYER_HEIGHT / 2;
			break;

		case MAZE_EXIT::RIGHT:
			x = MAZE_XMAX - PLAYER_WIDTH;
			y = MAZE_YCENTER - Player::PLAYER_HEIGHT / 2;
			break;

		default:
			break;
	}

	setPos(x, y);
}

void Player::move()
{
	if (IsElectrocuting() == false)
	{
		// move player left/right
		mPosX += mVelX;
		mCollider.x = mPosX;

		// move player up/down
		mPosY += mVelY;
		mCollider.y = mPosY;
	}
}