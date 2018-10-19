// SDL2Berzerk.h

#ifndef __SDL2BERZERK__
#define __SDL2BERZERK__
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <map>
#include "Texture.h"

// screen / maze object dimensions
const int BORDERTHICKNESS = 20;
const int WALLTHICKNESS = 8;
const int MAZE_WIDTH = 640;
const int MAZE_HEIGHT = 480;
const int MAZE_XMIN = BORDERTHICKNESS + WALLTHICKNESS;
const int MAZE_YMIN = BORDERTHICKNESS + WALLTHICKNESS;
const int MAZE_XMAX = MAZE_XMIN + MAZE_WIDTH;
const int MAZE_YMAX = MAZE_YMIN + MAZE_HEIGHT;
const int MAZE_XCENTER = MAZE_XMIN + MAZE_WIDTH / 2;
const int MAZE_YCENTER = MAZE_YMIN + MAZE_HEIGHT / 2;
const int BORDER_HSEGMENT = MAZE_WIDTH / 5;
const int BORDER_VSEGMENT = MAZE_HEIGHT / 3;

// screen dimensions
const int SCREEN_WIDTH = BORDERTHICKNESS * 2 + WALLTHICKNESS * 2 + MAZE_WIDTH;
const int SCREEN_HEIGHT = BORDERTHICKNESS * 3 + WALLTHICKNESS * 3 + MAZE_HEIGHT;

const int SCREEN_FPS = 60;
const int SCREEN_TICK_PER_FRAME = 1000 / SCREEN_FPS;


//The window we'll be rendering to
extern SDL_Window* gWindow;

//The window renderer
extern SDL_Renderer* gRenderer;

// globally used font
extern TTF_Font * gFont;

// rendered texture
//extern Texture gTextTexture;

//Game Controller 1 handler with force feedback
extern SDL_Joystick* gGameController;
extern SDL_Haptic * gControllerHaptic;

enum HEADING { N = 0, S, E, W, NE, NW, SE, SW, _NONE };
enum MAZE_EXIT { NONE = 0x00, TOP = 0x01, BOTTOM = 0x02, LEFT = 0x04, RIGHT = 0x08 };

enum OBJ_TYPE { UNKNOWN, PLAYER, ROBOT, ROBOT_EXPLODE, OTTO, BULLET, LIFE, MAZEWALL, MAZEDOOR, SCORE };


// misc game constants
const int MAX_LIVES = 3;
const int MIN_ROBOTS = 4;
const int MAX_ROBOTS = 12;
const int ROBOT_KILL_POINTS = 50;
const int BONUS_LIFE_SCORE = 5000;

#endif

