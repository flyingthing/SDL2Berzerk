#include "Maze.h"

Maze::Maze(int roomX, int roomY)
{
	this->roomX = roomX;
	this->roomY = roomY;
	RoomNumber();

	exitsblock.clear();
}

Maze::~Maze()
{
}

std::list<HEADING> Maze::GetPillars()
{
	rng.seed = rmNumber;

	std::list<HEADING> pillars;

	for (int i = 0; i < 8; i++)
	{
		rng.getRandomNumber();
		pillars.push_back((HEADING)(rng.getRandomNumber() & 0x03));
	}

	return pillars;
}

void Maze::RoomNumber()
{
	this->rmNumber = (this->roomY << 8) + this->roomX;
}

int Maze::GetExitBlock()
{
	int exitflgs = 0;

	std::map<unsigned int, int>::iterator it = exitsblock.find(rmNumber);

	if (it != exitsblock.end())
	{
		exitflgs = exitsblock[rmNumber];
	}

	return exitflgs;
}

void Maze::SetExitBlock( MAZE_EXIT exit )
{
	int flg = GetExitBlock() | (int)exit;

	exitsblock[rmNumber] = flg;
}


RNG::RNG()
{
}

RNG::~RNG()
{
}

unsigned int RNG::randomNumber(unsigned int seed)
{
	this->seed = (seed * 7) + 0x3153;
	return (this->seed >> 8) & 0xFF;
}

unsigned int RNG::getRandomNumber()
{
	return randomNumber(this->seed);
}