#include "Game.h"
#include <sstream>
#include <iomanip>

#define BLACK        {  0,   0,   0, 255}
#define WHITE        {255, 255, 255, 255}
#define GREEN        {  0, 255,   0, 255}
#define RED          {255,   0,   0, 255}
#define BLUE         {  0,   0, 255, 255}
#define YELLOW       {255, 255,   0, 255}
#define CYAN         {  0, 255, 255, 255}
#define PURPLE       {255,   0, 255, 255}
#define DARK_ORCHID  {153,  50, 204, 255}
#define DARKYELLOW   {128, 128,   0, 255}
#define LIGHTSKYBLUE {135, 206, 250, 255}
#define GRAY         {190, 190, 190, 255}
#define BRIGHTBLUE   {  0,  50, 255, 255}
#define DEEPSKYBLUE  {  0, 191, 255, 255}
#define LIGHTYELLOW  {255, 255, 224, 255}
#define PALEGREEN    {152, 251, 152, 255}

#define PLAYER_COLOR		GREEN
#define SCORE_COLOR			GREEN
#define LIVES_COLOR			GREEN
#define WALL_COLOR			BLUE
#define SCROLL_OBJ_COLOR	BLUE

//enum COLORS { BLACK, WHITE, GREEN, RED, BLUE, YELLOW, CYAN, PURPLE, DARKORCHID, DARKYELLOW, LIGHTSKYBLUE, GRAY, BRIGHTBLUE, DEEPSKYBLUE, LIGHTYELLOW, PALEGREEN };


static LEVEL gamelevel[] = { 
	{YELLOW, false,   500 },		
	{RED,     true,  1500 },		
	{CYAN,    true,  3000 },		
	{GREEN,   true,  4500 },
	{PURPLE,  true,  6000 },
	{YELLOW,  true,  7500 },
	{WHITE,   true, 10000 },
	{LIGHTSKYBLUE,   true, 11000 },
	{PURPLE,  true, 13000 },
	{GRAY,    true, 15000 },
	{YELLOW,  true, 17000 },
	{RED,     true, 19000 },
	{LIGHTSKYBLUE,   true, 999999 }
};


struct ROBOTBULLET
{
	Bullet * bullet;
	Robot * robot;
};

int Robot::objectCount = 0;

//Box collision detector
bool checkWallCollision(AnimateObj *, std::list<Wall *>);

static Uint32 GameOverX(Uint32 interval, void *param)
{
	UserEvent(GAME_OVER, NULL, NULL);
	return 0;
}

static void RobotKill(void *p)
{
	UserEvent(ROBOT_KILL, p, NULL);
}

static Uint32 PlayerKill(Uint32 interval, void *param)
{
	UserEvent(PLAYER_ELECTROCUTED, NULL, NULL);
	return 0;
}

static Uint32 RobotWakeUp(Uint32 interval, void *param)
{
	Game *game = (Game *)param;
	SDL_RemoveTimer(game->robotactive_timer_id);
	UserEvent(ROBOT_ACTIVE, NULL, NULL);
	return 0;
}

static Uint32 OttoWakeUp(Uint32 interval, void *param)
{
	Game *game = (Game *)param;
	SDL_RemoveTimer(game->ottoactive_timer_id);
	UserEvent(OTTO_SPAWN, NULL, NULL);
	return 0;
}

static void OttoCallBack(void *p)
{
	UserEvent(OTTO_ACTIVE, NULL, NULL);
}

Game::Game(Texture * gTextTexture, std::map<OBJ_TYPE, Texture *> TextureSpriteSheets)
{
	//LoadSprites();
	this->TextureSpriteSheets = TextureSpriteSheets;

	maze = new Maze(0x53, 0x31);
	this->gTextTexture = gTextTexture;

	player = new Player(this->TextureSpriteSheets[OBJ_TYPE::PLAYER], PLAYER_COLOR, NULL);

	for (int i = 0; i < 3; i++)
	{
		Lives *lives = new Lives(this->TextureSpriteSheets[OBJ_TYPE::LIFE], LIVES_COLOR);
		lives->setPos(SCREEN_WIDTH / 4 + (i*Lives::LIVES_WIDTH + Lives::LIVES_WIDTH), SCREEN_HEIGHT - Lives::LIVES_HEIGHT - WALLTHICKNESS);
		livesleft.push_back(lives);
	}

	score = new Score(gTextTexture, SCORE_COLOR);
}

Game::~Game()
{
	/*
	robotSpriteSheet.free();
	robotexpodeSpriteSheet.free();
	bulletSpriteSheet.free();
	ottoSpriteSheet.free();
	livesSpriteSheet.free();
	playerSpriteSheet.free();
	ottoSpriteSheet.free();
	*/
	TextureSpriteSheets.clear();

	delete player;
	delete score;
}



bool Game::Run()
{
	LEVEL * level = NULL;

	for (int l = 0;; l++)
	{
		if (score->GameScore() < gamelevel[l].maxscore)
		{
			level = &gamelevel[l];
			break;
		}
	}

	gameover = false;

	// main loop flag
	bool quit = false;

	//Event handler
	SDL_Event e;

	/* initialize random seed: */
	srand(time(NULL));

	//player->reset(DOOR::LEFT);
	otto = NULL;
	int otto_startx = player->mPosX;
	int otto_starty = player->mPosY;
	objs.push_back(player);

	objs.push_back(score);

	std::list<Lives *>::iterator it = livesleft.begin();
	for (int i = 0; i < livesleft.size() - 1; i++)
	{
		objs.push_back(*it);
		++it;
	}

	MazeExits(level);
	Walls();

	Robot::objectCount = rand() % MAX_ROBOTS;
	if (Robot::objectCount < MIN_ROBOTS)
	{
		Robot::objectCount = MIN_ROBOTS;
	}

	for (int i = 0; i < Robot::objectCount; i++)
	{
		Robot * robot = new Robot(TextureSpriteSheets[OBJ_TYPE::ROBOT], level->color, NULL);

		int old_h = robot->mCollider.h;
		int old_w = robot->mCollider.w;
		robot->mCollider.h *= 2;
		robot->mCollider.w *= 2;

		bool collflg = false;
		do
		{
			int xpos = rand() % (640 - 32) + 16; // range 0-640
			int ypos = rand() % (480 - 64) + 32;
			robot->setPos(xpos, ypos);

			// check robot 2 robot collision here
			collflg = false;
			for (Robot * irobot : robots)
			{

				collflg = checkCollision(robot->mCollider, irobot->mCollider);
				if (collflg == true)
				{
					break;
				}
			}

			if (collflg == true)
			{
				continue;
			}

		} while (checkWallCollision(robot, walls) == true || collflg == true);

		robot->mCollider.h = old_h;
		robot->mCollider.w = old_w;
		robots.push_back(robot);
		objs.push_back(robot);
	}

	// flush any user events from queue
	SDL_FlushEvents(SDL_USEREVENT, SDL_LASTEVENT);

	robotactive_timer_id = SDL_AddTimer(250 * Robot::objectCount, RobotWakeUp, this);
	ottoactive_timer_id = SDL_AddTimer(1000 * Robot::objectCount, OttoWakeUp, this);	// Fix this!!!...when to send Otto?


	ClockTimer fpsTimer;	// frames per second timer
	ClockTimer capTimer;	// FPS cap timer

	// in memory text stream
	std::stringstream timeText;

	// start counting frames per second
	int countedFrames = 0;
	fpsTimer.start();

	// application is running
	while (!quit)
	{
		// start cap timer
		capTimer.start();

		// handle events on queue
		while (SDL_PollEvent(&e) != 0)
		{
			//User requests quit
			if (e.type == SDL_QUIT)
			{
				quit = true;
			}

			if (e.type == SDL_USEREVENT)
			{
				switch (e.user.code)
				{
				case PLAYER_ROBOT_COLLISION:
				{
					// FIX LOGIC!!! player delete, etc
					Robot * robot = (Robot *)e.user.data2;

					RobotExplode *explode = new RobotExplode(TextureSpriteSheets[OBJ_TYPE::ROBOT_EXPLODE], RobotKill, robot);
					//objs.push_back(explode);
					objs.push_front(explode);
					robotsexplode.push_back(explode);
					robots.remove(robot);
					objs.remove(robot);
					delete robot;
					score->robotkill();
				}
				break;

				case ROBOT_ACTIVE:
					//SDL_RemoveTimer(robotactive_timer_id);
					robotactive_timer_id = 0;
					for (Robot * robot : robots)
					{
						robot->activate(true);
					}
					break;

				case OTTO_SPAWN:
				{
					//SDL_RemoveTimer(ottoactive_timer_id);
					ottoactive_timer_id = 0;
					otto = new Otto(TextureSpriteSheets[OBJ_TYPE::OTTO], level->color, OttoCallBack, player);
					otto->setPos(otto_startx, otto_starty);
					objs.push_back(otto);
				}
					break;

				case OTTO_ACTIVE:
					otto->setPattern("otto");
					otto->activate(true);
					break;

				case PLAYER_EXIT_MAZE:
					ExitMaze(player, (MAZE_EXIT)((int)e.user.data1));
					quit = true;
					break;

				case PLAYER_WALL_COLLISION:
				case PLAYER_OTTO_COLLISION:
				case PLAYER_ROBOTEXPLODE_COLLISION:
				case OTTO_PLAYER:
				{
					player->setPattern("electrocuting");
					timer_id = SDL_AddTimer(1500, PlayerKill, NULL);

					//Play rumble at 75% strenght for 500 milliseconds
					/*
					if (SDL_HapticRumblePlay(gControllerHaptic, 0.75, 500) != 0)
					{
						printf("Warning: Unable to play rumble! %s\n", SDL_GetError());
					}
					*/
				}
					break;

				case PLAYER_ELECTROCUTED:
					SDL_RemoveTimer(this->timer_id);

					if (livesleft.size() > 1)
					{
						objs.remove(livesleft.back());
						livesleft.pop_back();

						quit = true;
						player->reset(PLAYER_COLOR, MAZE_EXIT::LEFT);

						// randomly pick a new room
						int x = rand() % 3 - 1;  //range: -1, 0, 1
						int y = rand() % 3 - 1;
						maze->roomX = (maze->roomX + x) & 0xFF;
						maze->roomY = (maze->roomY + y) & 0xFF;
						maze->RoomNumber();
					}
					else
					{
						// GameOver
						timer_id = SDL_AddTimer(100, GameOverX, NULL);
						objs.remove(player);
					}

					break;

				case GAME_OVER:
					SDL_RemoveTimer(this->timer_id);
					quit = true;
					gameover = true;
					break;

				case ROBOT_OTTO_COLLISION:
				{
					Robot * robot = (Robot *)e.user.data1;
				}
					break;

				case ROBOT_KILL:
				{
					RobotExplode * robot = (RobotExplode *)e.user.data1;
					objs.remove(robot);
					robotsexplode.remove(robot);
					delete robot;
				}
				break;

				case OTTO_ROBOT:
				case ROBOT_WALL_COLLISION:
				case ROBOT_ROBOTEXPLODE_COLLISIONS:
				{
					Robot *robot = (Robot *)e.user.data1;

					RobotExplode *explode = new RobotExplode(TextureSpriteSheets[OBJ_TYPE::ROBOT_EXPLODE], RobotKill, robot);
					//objs.push_back(explode);
					objs.push_front(explode);
					robotsexplode.push_back(explode);

					robots.remove(robot);
					objs.remove(robot);
					delete robot;
					score->robotkill();
				}
				break;

				case PLAYER_BULLET_FIRE:
				{
					HEADING dir = (HEADING)(int)e.user.data1;
					if (dir != HEADING::_NONE)
					{
						Bullet *bullet = new Bullet(TextureSpriteSheets[OBJ_TYPE::BULLET], level->color, dir);
						bullet->setColor(level->color);
						bullet->setPos(player->mPosX, player->mPosY);

						// set velocity

						bullets.push_back(bullet);
						objs.push_back(bullet);
					}
				}
					break;

				case ROBOT_BULLET_FIRE:
				{
					Robot *robot = (Robot *)e.user.data1;
					//Bullet *bullet = new Bullet("bullets2.png", gRenderer, NULL);
					//bullet->setPos(robot->mPosX, robot->mPosY);

					
					// set velocity

					//objs.push_back(bullet);
				}
					break;

				case BULLET_KILL:
				{
					Bullet *bullet = (Bullet *)e.user.data1;
					bullets.remove(bullet);
					objs.remove(bullet);
					delete bullet;
				}
				break;

				case ROBOT_BULLET_COLLISION:
				{
					Robot *robot = (Robot *)e.user.data1;
					Bullet *bullet = (Bullet *)e.user.data2;

					RobotExplode *explode = new RobotExplode(TextureSpriteSheets[OBJ_TYPE::ROBOT_EXPLODE], RobotKill, robot);
					objs.push_back(explode);
					robotsexplode.push_back(explode);

					robots.remove(robot);
					objs.remove(robot);
					delete robot;

					bullets.remove(bullet);
					objs.remove(bullet);
					delete bullet;

					score->robotkill();
				}
				break;

				default:
					break;
				}
			}
			else
			{
				// handle input
				player->handleEvent(e);
			}
		}

		if (!quit)
		{
			// move objects
			for (AnimateObj *obj : objs)
			{
				if (obj->obj_type == ROBOT)
					obj->move(player->mPosX, player->mPosY);
				else
					obj->move();
			}

// https://gamedevelopment.tutsplus.com/tutorials/quick-tip-use-quadtrees-to-detect-likely-collisions-in-2d-space--gamedev-374
// https://www.packtpub.com/mapt/book/Game%20Development/9781849696821/2/ch02lvl1sec15/Source+and+destination+rectangles
// http://www.cplusplus.com/forum/windows/148218/

			// wall collisions
			for (Wall * wall : walls)
			{
				if (player->IsElectrocuting() == false)
				{
					if (checkCollision(player->mCollider, wall->mCollider) == true)
					{
						if (wall->obj_type == OBJ_TYPE::MAZEWALL)
							UserEvent(PLAYER_WALL_COLLISION, (void *)player, (void *)wall);
						else
						{
							Door * door = (Door *)wall;

							if (door->GetState() == true)
							{
								UserEvent(PLAYER_WALL_COLLISION, (void *)player, (void *)wall);
							}
							else
								UserEvent(PLAYER_EXIT_MAZE, (void *)door->GetExit(), NULL);
						}
					}
				}

				for (Robot * robot : robots)
				{
					if (checkCollision(robot->mCollider, wall->mCollider) == true)
					{
						if (robot->deltag == false)
						{
							UserEvent(ROBOT_WALL_COLLISION, (void *)robot, NULL);
							robot->deltag = true;
						}
					}
				}

				for (Bullet * bullet : bullets)
				{
					if (bullet->deltag == false)
					{
						if (checkCollision(bullet->mCollider, wall->mCollider) == true)
						{
							//printf("BULLET %0x\n", bullet);
							bullet->deltag = true;
							UserEvent(BULLET_KILL, (void *)bullet, NULL);
						}
					}
				}
			}

			// bullet collisions
			for (Bullet * bullet : bullets)
			{
				if (bullet->deltag == false)
				{
					if (player->IsElectrocuting() == false)
					{
						if (checkCollision(player->mCollider, bullet->mCollider) == true)
						{
							UserEvent(PLAYER_BULLET_COLLISION, (void *)player, (void *)bullet);
						}
					}

					for (Robot * robot : robots)
					{
						if (checkCollision(bullet->mCollider, robot->mCollider) == true)
						{
							UserEvent(ROBOT_BULLET_COLLISION, (void *)robot, (void *)bullet);
						}
					}

					if (otto != NULL)
					{
						if (checkCollision(bullet->mCollider, otto->mCollider) == true)
						{
							UserEvent(BULLET_KILL, (void *)bullet, NULL);
						}
					}
				}
			}

			// player-robot collisions
			if (player->IsElectrocuting() == false)
			{
				for (Robot * robot : robots)
				{
					if (checkCollision(player->mCollider, robot->mCollider) == true)
					{
						UserEvent(PLAYER_ROBOT_COLLISION, (void *)player, (void *)robot);
					}
				}

				// player-robotexplode collisions
				for (RobotExplode *robot : robotsexplode)
				{
					if (checkCollision(player->mCollider, robot->mCollider) == true)
					{
						UserEvent(PLAYER_ROBOTEXPLODE_COLLISION, (void *)player, (void *)robot);
					}
				}

				if (otto != NULL)
				{
					if (checkCollision(player->mCollider, otto->mCollider) == true)
					{
						UserEvent(OTTO_PLAYER, NULL, NULL);
					}
				}

			}

			// robot-robot collisions
			for (Robot * robot : robots)
			{
				/* FIX THIS!!!
				if (checkCollision(robotex->mCollider, robot->mCollider) == true)
				{
					UserEvent(ROBOT_ROBOTEXPLODE_COLLISIONS, (void *)robot, (void *)robotex);
				}
				*/
		
				// robot-robotexplode collisions
				for (RobotExplode *robotex : robotsexplode)
				{
					if (checkCollision(robot->mCollider, robotex->mCollider) == true)
					{
						UserEvent(ROBOT_ROBOTEXPLODE_COLLISIONS, (void *)robot, (void *)robotex);
					}
				}

				if (otto != NULL)
				{
					if (checkCollision(robot->mCollider, otto->mCollider) == true)
					{
						UserEvent(OTTO_ROBOT, (void *)robot, NULL);
					}
				}
			}

			/*
			for (std::list<Robot *>::iterator robotIter = robots.begin(); robotIter != robots.end(); ++robotIter)
			{
			if (checkCollision((*robotIter)->mCollider, wall) == true)
			{
			UserEvent(WALL_COLLISION);
			//printf("COLLISION!");
			}
			}
			*/


			// calculate and correct fps
			float avgFPS = countedFrames / (fpsTimer.getTicks() / 1000.f);
			if (avgFPS > 2000000)
			{
				avgFPS = 0;
			}

			// set text to be rendered
			timeText.str("");
			//timeText << "Average Frames Per Second " << avgFPS;
			timeText << std::setprecision(4) << avgFPS;

			//SDL_Color textColor = { 0, 0xff, 0 };

			//Render text
			if (!gTextTexture->loadFromRenderedText(timeText.str().c_str(), { 0x0, 0xFF, 0x00 }))
			{
				printf("Unable to render FPS texture!\n");
			}

			//Clear screen
			SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
			SDL_RenderClear(gRenderer);


			//Render textures
			gTextTexture->render((SCREEN_WIDTH - gTextTexture->getWidth()) / 2, (SCREEN_HEIGHT - gTextTexture->getHeight()));


			// render objects
			for (AnimateObj *obj : objs)
			{
				obj->render();
			}

			//DrawGrid();
			DrawBoard();

			// update screen
			SDL_RenderPresent(gRenderer);
			++countedFrames;

			// if frame finished early
			int frameTicks = capTimer.getTicks();
			if (frameTicks < SCREEN_TICK_PER_FRAME)
			{
				// wait remaining time
				SDL_Delay(SCREEN_TICK_PER_FRAME - frameTicks);
			}
		}
	}

	Quit();

	if (gameover)
		GameOver();

	return gameover;
}

void Game::DrawGrid()
{
	int CELLSIZE = 20;
	

	SDL_SetRenderDrawColor(gRenderer, 190, 190, 190, 255);	// GRAY

	for (int x = MAZE_XMIN; x <= MAZE_XMAX; x += 16)
	{ 
		// draw vertical line
		SDL_RenderDrawLine(gRenderer, x, MAZE_YMIN, x, MAZE_YMAX);
		
		for (int y = MAZE_YMIN; y <= MAZE_YMAX + 22; y += 22)
		{
			// draw horizontal lines
			SDL_RenderDrawLine(gRenderer, MAZE_XMIN, y, MAZE_XMAX, y);
		}
	}
}

void Game::DrawBoard() 
{
	// draw current line
	int TILEW = 16;
	int TILEH = 16;
	int WINW = 640;
	int WINH = 480;

	SDL_Surface * dead_surface = SDL_LoadBMP("dead.bmp");
	SDL_Texture * dead = SDL_CreateTextureFromSurface(gRenderer, dead_surface);

	/* Since we just converted the surfaces to textures, we no longer need those
	* surfaces and we can free them.
	*/
	SDL_FreeSurface(dead_surface);


	int num_tiles_x = (float(WINW) / TILEW) + ((WINW%TILEW) ? 1 : 0);
	int num_tiles_y = (float(WINH) / TILEH) + ((WINH%TILEH) ? 1 : 0);

	/* This is the source rectangle, this chooses what part of the image
	* will be rendered. Currently, all of our tiles are the same size
	* and in different files. So the source rectangle will
	* never change.
	*/
	SDL_Rect src{ 0, 0, TILEW, TILEH };

	for (int tile_y = 0; tile_y < num_tiles_y; tile_y++) {
		for (int tile_x = 0; tile_x < num_tiles_x; tile_x++) {

			/* This is the destination rectangle. This is where and at what
			* size the image will be placed on the screen. The destination
			* rectangle changes per-tile, as the location also changes
			* per-tile.
			*/
			/*
			SDL_Rect dst{
				(tile_x*TILEW),
				(tile_y*TILEH),
				TILEW, TILEH
			};



			SDL_RenderCopy(gRenderer, dead, &src, &dst);
			*/

			//SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
			//SDL_RenderDrawLine(gRenderer, tile_x*TILEW, tile_y*TILEH, MAZE_XMAX, MAZE_YMAX);

		}
	}

	SDL_DestroyTexture(dead);
}

// GameOver
void Game::GameOver()
{
	// FIX: wait for player electrocute to finish and robot explode to finish

	// Change this to a game object???
	gameover = true;
	objs.remove(player);
	SDL_Color textColor = { 0x00, 0xFF, 0x00 };
	if (!gTextTexture->loadFromRenderedText("GAME OVER", textColor))
	{
		printf("Failed to render text texture!\n");
	}
	gTextTexture->render((SCREEN_WIDTH - gTextTexture->getWidth()) / 2, (SCREEN_HEIGHT - gTextTexture->getHeight()) / 2);
	//Update screen
	SDL_RenderPresent(gRenderer);
	for (;;);
}


void Game::ExitMaze(Player *player, MAZE_EXIT exit)
{
	for (AnimateObj *obj : objs)
	{
		switch (obj->obj_type)
		{
			case OBJ_TYPE::PLAYER:
			case OBJ_TYPE::ROBOT:
			case OBJ_TYPE::ROBOT_EXPLODE:
			case OBJ_TYPE::OTTO:
			case OBJ_TYPE::BULLET:
			case OBJ_TYPE::MAZEWALL:
				obj->setColor(SCROLL_OBJ_COLOR);
				obj->render();
				break;

			case OBJ_TYPE::MAZEDOOR:
			{
				Door * door = (Door *)obj;

				if (door->GetState() == true)
				{
					obj->setColor(SCROLL_OBJ_COLOR);
					obj->render();
				}
			}
				break;

			default:
				break;
		}
	}

	scrollMaze(exit);

	int exitflgs = maze->GetExitBlock() | (int)exit;
	maze->SetExitBlock(exit);

	MAZE_EXIT exitdoor = MAZE_EXIT::NONE;

	switch (exit)
	{
	case MAZE_EXIT::BOTTOM: maze->roomY--; exitdoor = MAZE_EXIT::TOP;  break;
	case MAZE_EXIT::RIGHT: maze->roomX--; exitdoor = MAZE_EXIT::LEFT;  break;
	case MAZE_EXIT::LEFT: maze->roomX++; exitdoor = MAZE_EXIT::RIGHT;  break;
	case MAZE_EXIT::TOP: maze->roomY++; exitdoor = MAZE_EXIT::BOTTOM;  break;
	default: break;
	}

	player->reset(PLAYER_COLOR, exitdoor);
	maze->RoomNumber();
	maze->SetExitBlock(exitdoor);

	//Update screen
	//SDL_RenderPresent(gRenderer);
}

void Game::scrollMaze(MAZE_EXIT exit)
{
	bool ret = saveScreenShotBMP("MyBMP.bmp", gWindow, gRenderer);

	SDL_Surface * loadsurf = SDL_LoadBMP("MyBMP.bmp");
	SDL_Texture* Background_Tx = SDL_CreateTextureFromSurface(gRenderer, loadsurf);
	SDL_FreeSurface(loadsurf);

	int scrollingOffset = 0;
	bool scroll = true;

	ClockTimer fpsTimer;	// frames per second timer
	ClockTimer capTimer;	// FPS cap timer
	fpsTimer.start();
	//capTimer.start();

	while (scroll)
	{
		scrollingOffset += 5;

		//Clear screen
		SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
		SDL_RenderClear(gRenderer);

		SDL_Rect srcR;
		srcR.x = 0;
		srcR.y = 0;
		srcR.w = SCREEN_WIDTH;
		srcR.h = SCREEN_HEIGHT;

		SDL_Rect dstR;
		dstR.x = 0;
		dstR.y = 0;
		dstR.w = SCREEN_WIDTH;
		dstR.h = SCREEN_HEIGHT;

		switch (exit)
		{
		case MAZE_EXIT::TOP:	// scrolls down
			srcR.h -= (40 + scrollingOffset);
			dstR.y = scrollingOffset; dstR.h -= (40 + scrollingOffset);
			if (scrollingOffset >= SCREEN_HEIGHT)
			{
				scroll = false;
			}
			break;

		case MAZE_EXIT::BOTTOM:	// scrolls up
			srcR.y = scrollingOffset; srcR.h -= (40 + scrollingOffset);
			dstR.h -= (40 + scrollingOffset);
			if (scrollingOffset >= SCREEN_HEIGHT)
			{
				scroll = false;
			}
			break;

		case MAZE_EXIT::LEFT:	// scrolls right
			srcR.w -= scrollingOffset; srcR.h = MAZE_YMAX + WALLTHICKNESS;
			dstR.x = scrollingOffset; dstR.w -= scrollingOffset; dstR.h = MAZE_YMAX + WALLTHICKNESS;
			if (scrollingOffset >= SCREEN_WIDTH)
			{
				scroll = false;
			}
			break;

		case MAZE_EXIT::RIGHT:	// scrolls left
			srcR.x = scrollingOffset; srcR.h = MAZE_YMAX + WALLTHICKNESS;
			dstR.w -= scrollingOffset; dstR.h = MAZE_YMAX + WALLTHICKNESS;
			if (scrollingOffset >= SCREEN_WIDTH)
			{
				scroll = false;
			}
			break;

		default:
			break;
		}

		SDL_RenderCopyEx(gRenderer, Background_Tx, &srcR, &dstR, 0, NULL, SDL_FLIP_NONE);

		std::list<Lives *>::iterator it = livesleft.begin();
		for (int i = 0; i < livesleft.size() - 1; i++)
		{
			(*it)->render();
			++it;
		}

		score->render();

		//Update screen
		SDL_RenderPresent(gRenderer);

		// if frame finished early
		int frameTicks = capTimer.getTicks();
		//printf("Ticks: %d\n", frameTicks);
		if (frameTicks < SCREEN_TICK_PER_FRAME)
		{
			//printf("Delay: %d\n", SCREEN_TICK_PER_FRAME - frameTicks);
			// wait remaining time
			//SDL_Delay(SCREEN_TICK_PER_FRAME - frameTicks);
			SDL_Delay(15);
		}
	}

	SDL_DestroyTexture(Background_Tx);
}


void Game::Quit()
{
	if (ottoactive_timer_id != 0)
		SDL_RemoveTimer(ottoactive_timer_id);
	else
		otto->activate(false);

	if (robotactive_timer_id != 0)
		SDL_RemoveTimer(robotactive_timer_id);
	else
	{
		for (Robot * robot : robots)
			robot->activate(false);
	}

	objs.clear();

	for (Robot * robot : robots)
	{
		delete robot;
	}

	robots.clear();

	for (RobotExplode * robot : robotsexplode)
	{
		delete robot;
	}

	robotsexplode.clear();

	for (Wall * wall : walls)
	{
		delete wall;
	}
	walls.clear();

	delete otto;
}

// buggy maze pillars
// NNWSSWNN
//

void Game::Walls()
{
	std::list<std::list<int>> tmp = { { BORDERTHICKNESS, BORDERTHICKNESS, WALLTHICKNESS, BORDER_VSEGMENT + WALLTHICKNESS*2 },
	{ BORDERTHICKNESS, BORDERTHICKNESS + WALLTHICKNESS + BORDER_VSEGMENT * 2, WALLTHICKNESS, BORDER_VSEGMENT },
	{ MAZE_XMAX, BORDERTHICKNESS, WALLTHICKNESS, BORDER_VSEGMENT + WALLTHICKNESS*2 },
	{ MAZE_XMAX, BORDERTHICKNESS + WALLTHICKNESS + BORDER_VSEGMENT * 2, WALLTHICKNESS, BORDER_VSEGMENT },
	{ BORDERTHICKNESS, BORDERTHICKNESS, WALLTHICKNESS + BORDER_HSEGMENT * 2, WALLTHICKNESS },
	{ MAZE_XMIN + BORDER_HSEGMENT * 3, BORDERTHICKNESS, BORDER_HSEGMENT * 2 + WALLTHICKNESS, WALLTHICKNESS },
	{ BORDERTHICKNESS, MAZE_YMAX, BORDER_HSEGMENT * 2 + WALLTHICKNESS, WALLTHICKNESS },
	{ MAZE_XMIN + BORDER_HSEGMENT * 3, MAZE_YMAX, BORDER_HSEGMENT * 2 + WALLTHICKNESS, WALLTHICKNESS }
	};

	for (std::list<int> xx : tmp)
	{
		int x = xx.front(); xx.pop_front();
		int y = xx.front(); xx.pop_front();
		int w = xx.front(); xx.pop_front();
		int h = xx.front(); xx.pop_front();
		Wall * wall = new Wall(x, y, w, h, WALL_COLOR, gRenderer);
		objs.push_back(wall);
		walls.push_back(wall);
	}

	int fx[] = { 1,2,3,4,1,2,3,4 };
	int fy[] = { 1,1,1,1,2,2,2,2 };
	int i = 0;

	std::list<HEADING>pillars = maze->GetPillars();

	for (HEADING pillar : pillars)
	{
		int x = 0, y = 0, w = 0, h = 0;

		x = MAZE_XMIN + BORDER_HSEGMENT*fx[i];
		y = MAZE_YMIN + BORDER_VSEGMENT*fy[i];

		switch (pillar)
		{
		case N:
			printf("N");
			w = WALLTHICKNESS;
			h = BORDER_VSEGMENT + WALLTHICKNESS;
			//y -= (BORDER_VSEGMENT + WALLTHICKNESS);
			y -= BORDER_VSEGMENT;
			//if (i > 3)
				//y += WALLTHICKNESS;
			//y -= BORDER_VSEGMENT;
			break;

		case S:
			printf("S");
			w = WALLTHICKNESS;
			h = BORDER_VSEGMENT + WALLTHICKNESS;
			break;

		case E:
			printf("E");
			w = BORDER_HSEGMENT;
			h = WALLTHICKNESS;
			break;

		case W:
			printf("W");
			w = BORDER_HSEGMENT;
			h = WALLTHICKNESS;
			x -= BORDER_HSEGMENT;
			break;

		default:
			break;
		}

		i++;

		Wall * wall = new Wall(x, y, w, h, WALL_COLOR, gRenderer);
		objs.push_back(wall);
		walls.push_back(wall);
	}

	printf("\n");
}


void Game::MazeExits(LEVEL * level)
{
	int exitflgs = maze->GetExitBlock();

	for (int i = 0x01; i < 0x0F; i <<= 1)
	{
		int x = 0;
		int y = 0;
		int w = 0;
		int h = 0;
		bool state = false;
		SDL_Color color = BLACK;

		switch ((MAZE_EXIT)i)
		{
		case MAZE_EXIT::TOP:
			x = MAZE_XMIN + BORDER_HSEGMENT * 2;
			y = MAZE_YMIN;
			w = BORDER_HSEGMENT;
			h = WALLTHICKNESS / 2;

			if (i & exitflgs)
			{
				y -= (WALLTHICKNESS * 3) / 4;
				color = level->color;
				state = true;
			}
			else
			{
				y -= WALLTHICKNESS * 3;
			}
			break;

		case MAZE_EXIT::BOTTOM:
			x = MAZE_XMIN + BORDER_HSEGMENT * 2;
			y = MAZE_YMAX;
			w = BORDER_HSEGMENT;
			h = WALLTHICKNESS / 2;

			if (i & exitflgs)
			{
				y += WALLTHICKNESS / 4;
				color = level->color;
				state = true;
			}
			else
			{
				y += WALLTHICKNESS * 2;
			}
			break;

		case MAZE_EXIT::LEFT:
			x = MAZE_XMIN;
			y = MAZE_YMIN + BORDER_VSEGMENT;
			w = WALLTHICKNESS / 2;
			h = BORDER_VSEGMENT;

			if (i & exitflgs)
			{
				x -= (WALLTHICKNESS * 3) / 4;
				color = level->color;
				state = true;
			}
			else
			{
				x -= WALLTHICKNESS * 3;
			}
			break;

		case MAZE_EXIT::RIGHT:
			x = MAZE_XMAX;
			y = MAZE_YMIN + BORDER_VSEGMENT;
			w = WALLTHICKNESS / 2;
			h = BORDER_VSEGMENT;

			if (i & exitflgs)
			{
				x += WALLTHICKNESS/4;
				color = level->color;
				state = true;
			}
			else
			{
				x += WALLTHICKNESS * 2;
			}
			break;

		default:
			break;
		}

		if ( i <= 0x0F )
			Exit(state, (MAZE_EXIT)i, x, y, w, h, color);
	}
}

void Game::Exit(bool state, MAZE_EXIT exit, int x, int y, int w, int h, SDL_Color color)
{
	Door * door = new Door(state, exit, x, y, w, h, color, gRenderer);
	walls.push_back(door);
	objs.push_back(door);
}

bool checkWallCollision(AnimateObj *obj, std::list<Wall *> walls)
{
	bool bRet = false;

	for (Wall * wall : walls)
	{
		bRet = checkCollision(obj->mCollider, wall->mCollider);

		if (bRet == true)
		{
			//UserEvent(WALL_COLLISION, (void *)obj, NULL);
			break;
		}
	}

	return bRet;
}



