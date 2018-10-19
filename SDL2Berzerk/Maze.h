// Maze.h

#ifndef __MAZE__
#define __MAZE__
#include "SDL2Berzerk.h"
#include <list>
#include <map>

struct RMDOORS
{
	unsigned int rmNumber;
	int doors;
};

class RNG
{
public:
	RNG();
	~RNG();

	unsigned int seed = 0;

	unsigned int randomNumber(unsigned int);
	unsigned int getRandomNumber();
};

class Maze
{
public:
	Maze(int roomX, int roomY);
	~Maze();

	std::list<HEADING> GetPillars();

	void RoomNumber();

	int GetExitBlock();
	void SetExitBlock( MAZE_EXIT );

	unsigned int seed = 0;
	int roomX;
	int roomY;
	unsigned int rmNumber;

	std::map<unsigned int, int> exitsblock;

	RNG rng;

private:
};


#endif

