#include "RobotExplode.h"

RobotExplode::RobotExplode(Texture * pTexture, void (*cb)(void *), Robot * robot) : AnimateObj(OBJ_TYPE::ROBOT_EXPLODE, pTexture, robot->getColor(), ROBOTEXPLODE_WIDTH, ROBOTEXPLODE_HEIGHT, cb)
{
	//Initialize the offsets
	//mPosX = 60;
	//mPosY = 60;

	//Initialize the velocity
	mVelX = 0;
	mVelY = 0;

	setPos(robot->mPosX, robot->mPosY);

	addPattern("explode", { 0,1,2 });
	setPattern("explode");

	setFrame(0);
}

RobotExplode::~RobotExplode()
{
}
