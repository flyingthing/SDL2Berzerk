#include "Otto.h"
#include <complex>


Otto::Otto(Texture *pTexture, SDL_Color color, void(*cb)(void *), Player *player) : AnimateObj(OBJ_TYPE::OTTO, pTexture, color, OTTO_WIDTH, OTTO_HEIGHT, cb)
{
	//Initialize the offsets
	//mPosX = 60;
	//mPosY = 60;

	this->player = player;

	//Initialize the velocity
	mVelX = 1; // randomnum();
	mVelY = 1;

	addPattern("spawn", { 0,1,2,3,4,5});
	addPattern("otto", { 5 });
	setPattern("spawn");

	frame = 0;
	setFrame(frame);
}

Otto::~Otto()
{
}

// How strong the gravity is.
const double GRAVITY_CONSTANT = 0.3; // 0.5; //0.3

//Percent of velocity maintained on a bounce.
const double BOUNCINESS = 3.0; //1.0

//

void Otto::move()
{
	if (isActive == false)
		return;

	std::complex<int> tmp, playerpos, ottopos;

	playerpos = std::complex<int>(player->mPosX, player->mPosY);
	ottopos = std::complex<int>(mPosX, mPosY);
	tmp = playerpos - ottopos;

	//sign = lambda x : (x > 0) - (x < 0)
	
	int x = tmp.real(); // 3 * sign (tmp.real);
	int y = tmp.imag(); // 3 * sign(tmp.imag);


	mPosX += delta_x;
	mPosY += delta_y;

	delta_y -= GRAVITY_CONSTANT;

		
	if (delta_y <= -8.0)
		delta_y = -8.0;
	if (delta_y >= 6.0)
		delta_y = 6.0;

	//Figure out if we hit the left or right edge and need to reverse.
	if (mPosX < player->mPosX && delta_x < 0)
		delta_x *= -BOUNCINESS;
	else if (mPosX > player->mPosX && delta_x > 0)
		delta_x *= -BOUNCINESS;

	if (mPosY < player->mPosY && delta_y < 0)
	{
		if (delta_y * -1 > GRAVITY_CONSTANT * 1)
		{
			delta_y *= -BOUNCINESS;
		}
	}

	mCollider.x = mPosX;
	mCollider.y = mPosY;


		//printf("%d %f\n", mPosY, delta_y);

		/*
		//Move the dot left or right
		mPosX += mVelX;
		mCollider.x = mPosX;

		//If the dot went too far to the left or right
		if ((mPosX < 0) || (mPosX + OTTO_WIDTH > SCREEN_WIDTH))
		{
			//Move back
			mPosX -= mVelX;
			mCollider.x = mPosX;
		}

		//Move the dot up or down
		mPosY += mVelY;
		mCollider.y = mPosY;

		//If the dot went too far up or down
		if ((mPosY < 0) || (mPosY + OTTO_HEIGHT > SCREEN_HEIGHT))
		{
			//Move back
			mPosY -= mVelY;
			mCollider.y = mPosY;
		}
		*/
}
