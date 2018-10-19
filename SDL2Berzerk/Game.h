// Game.h

#ifndef __GAME__
#define __GAME__
#include <stdio.h>
#include <time.h>
#include <string>
#include <list>
#include <SDL.h>
#include "SDL2Berzerk.h"
#include "AnimateObj.h"
#include "Maze.h"
#include "Player.h"
#include "Robot.h"
#include "RobotExplode.h"
#include "Lives.h"
#include "Bullet.h"
#include "Utils.h"
#include "Score.h"
#include "Otto.h"
#include "Wall.h"
#include "Door.h"
#include "ClockTimer.h"

struct LEVEL
{
	/*
	LEVEL(std::map<char, int> a, bool b, int c) : fire(b), maxscore(c)
	{
		for (std::map<char,int>::iterator it = a.begin(); it != a.end(); ++it)
		{
			color[it->first] = it->second;
		}
	}
	*/

	SDL_Color color;
	bool fire;
	int maxscore;
};

class Game 
{
public:
	Game(Texture * gTextTexture, std::map<OBJ_TYPE, Texture *> TextureSpriteSheets);
	~Game();

	//void LoadSprites();
	bool Run();
	void Quit();
	void ExitMaze(Player *, MAZE_EXIT);
	void GameOver();

	Maze * maze = NULL;
	Uint32 timer_id = 0;
	Uint32 robotactive_timer_id = 0;
	Uint32 ottoactive_timer_id = 0;

	Texture * gTextTexture = NULL;

	/*
	Texture playerSpriteSheet;
	Texture robotSpriteSheet;
	Texture robotexpodeSpriteSheet;
	Texture bulletSpriteSheet;
	Texture ottoSpriteSheet;
	Texture livesSpriteSheet;
	*/
	std::map<OBJ_TYPE, Texture *> TextureSpriteSheets;

private:
	std::list<AnimateObj *> objs;
	std::list<Wall *> walls;
	std::list<Robot *> robots;
	std::list<RobotExplode *> robotsexplode;
	std::list<Lives *> livesleft;
	std::list<Bullet *> bullets;

	Player *player = NULL;
	Otto *otto = NULL;
	Score *score = NULL;

	void scrollMaze(MAZE_EXIT);

	void Walls();
	void MazeExits(LEVEL *);
	void Exit(bool, MAZE_EXIT, int, int, int, int, SDL_Color);

	void DrawBoard();
	void DrawGrid();

	bool gameover;
};
#endif

