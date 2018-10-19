#include "Robot.h"
#include <time.h>

static int randomnum()
{
	srand(time(NULL));

	int randNum = (rand() % 3) + 1;		// 1,2,3

	return randNum - 2;
}

Robot::Robot(Texture *pTexture, SDL_Color color, void (*cb)(void *)) : AnimateObj(OBJ_TYPE::ROBOT, pTexture, color, ROBOT_WIDTH, ROBOT_HEIGHT, cb)
{
	//Initialize the offsets
	//mPosX = 60;
	//mPosY = 60;

	//Initialize the velocity
	mVelX = -1; // randomnum();
	mVelY = randomnum();

	addPattern("roving_eye", { 0,1,2,1,0,5,4,5 });
	addPattern("E", { 7,6 });
	addPattern("W", { 10,11 });
	addPattern("N", { 12,13,12,14 });
	addPattern("S", { 0,8,0,9 });
	setPattern("roving_eye");

	frame = rand() % 8;
	setFrame(frame);
}

Robot::~Robot()
{
}


// FIX: robots move outside maze wall i.e. thru door

void Robot::move(int playerPosX, int playerPosY)
{
	if (isActive == true)
	{
		//Move the left or right
		mPosX += mVelX;
		mCollider.x = mPosX;

		//If the dot went too far to the left or right
		if ((mPosX < 0) || (mPosX + ROBOT_WIDTH > SCREEN_WIDTH))
		{
			//Move back
			mPosX -= mVelX;
			mCollider.x = mPosX;
		}

		//Move the dot up or down
		mPosY += mVelY;
		mCollider.y = mPosY;

		//If the dot went too far up or down
		if ((mPosY < 0) || (mPosY + ROBOT_HEIGHT > SCREEN_HEIGHT))
		{
			//Move back
			mPosY -= mVelY;
			mCollider.y = mPosY;
		}
	}
}