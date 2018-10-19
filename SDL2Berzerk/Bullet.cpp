#include "Bullet.h"


Bullet::Bullet(Texture *pTexture, SDL_Color color, HEADING heading) : AnimateObj(OBJ_TYPE::BULLET, pTexture, color, 0, 0, NULL)
{
	//Initialize the offsets
	//mPosX = 40;
	//mPosY = 40;

	//Initialize the velocity
	mVelX = 0;
	mVelY = 0;

	// modulate color(r,g,b) GREEN
	//setColor(0, 0xFF, 0x0);

	switch (heading)
	{
	case HEADING::N:
	{
		addPattern("N", { 3 });
		setPattern("N");
		setWidthHeight(2, 11);
		mVelX = 0;
		mVelY = -BULLET_VEL;
	}
		break;

	case HEADING::S:
	{
		addPattern("S", { 3 });
		setPattern("S");
		setWidthHeight(2, 11);
		mVelX = 0;
		mVelY = BULLET_VEL;
	}
	break;

	case HEADING::E:
	{
		addPattern("E", { 0 });
		setPattern("E");
		setWidthHeight(12, 2);
		mVelX = BULLET_VEL;
		mVelY = 0;
	}
	break;

	case HEADING::W:
	{
		addPattern("W", { 0 });
		setPattern("W");
		setWidthHeight(12, 2);
		mVelX = -BULLET_VEL;
		mVelY = 0;
	}
	break;

	case HEADING::NE:
	{
		addPattern("NE", { 2 });
		setPattern("NE");
		setWidthHeight(12, 11);
		mVelX = BULLET_VEL;
		mVelY = -BULLET_VEL;
	}
	break;

	case HEADING::NW:
	{
		addPattern("NW", { 1 });
		setPattern("NW");
		setWidthHeight(12, 11);
		mVelX = -BULLET_VEL;
		mVelY = -BULLET_VEL;
	}
	break;

	case HEADING::SE:
	{
		addPattern("SE", { 1 });
		setPattern("SE");
		setWidthHeight(12, 11);
		mVelX = BULLET_VEL;
		mVelY = BULLET_VEL;
	}
	break;

	case HEADING::SW:
	{
		addPattern("SW", { 2 });
		setPattern("SW");
		setWidthHeight(12, 11);
		mVelX = -BULLET_VEL;
		mVelY = BULLET_VEL;
	}
	break;

	default:
		break;

	}
}

Bullet::~Bullet()
{
}

int Bullet::xOffSet()
{
	std::string pattern = getPattern();
	int offset = 0;

	if (pattern.compare("N") == 0)
		offset = 18;
	else if (pattern.compare("S") == 0)
		offset = 18;
	else
		return AnimateObj::xOffSet();

	return offset;
}

void Bullet::move()
{
	// move the bullet left or right
	mPosX += mVelX;
	mCollider.x = mPosX;

	// if the bullet exited the maze
	if ((mPosX < MAZE_XMIN-WALLTHICKNESS) || (mPosX > MAZE_XMAX+WALLTHICKNESS))
	{
		this->deltag = true;
		UserEvent(USEREVENTS::BULLET_KILL, (void *)this, NULL);
		return;
	}

	// move the bullet up or down
	mPosY += mVelY;
	mCollider.y = mPosY;

	// if the bullet exited the maze
	if ((mPosY < MAZE_YMIN-WALLTHICKNESS) || (mPosY > MAZE_YMAX+WALLTHICKNESS))
	{
		this->deltag = true;
		UserEvent(USEREVENTS::BULLET_KILL, (void *)this, NULL);
		return;
	}
}
