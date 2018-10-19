//Using SDL, SDL_image, standard IO, and strings

#include "SDL2Berzerk.h"
#include <stdio.h>
#include "Game.h"
#include "Texture.h"


//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();
bool loadSprites();

//Frees media and shuts down SDL
void close();

void WindowIcon();

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;

// globally used font
TTF_Font * gFont = NULL;

// Rendered texture
Texture gTextTexture;

std::map<OBJ_TYPE, Texture *> TextureSpriteSheets;

//Game Controller 1 handler with force feedback
SDL_Joystick* gGameController = NULL;
SDL_Haptic * gControllerHaptic = NULL;



int main(int argc, char* args[])
{
	// start up SDL and create window
	if (!init())
	{
		printf("Failed to initialize!\n");
	}
	else
	{
		// load media
		if (!loadMedia())
		{
			printf("Failed to load media!\n");
		}
		else
		{
			Game game = Game(&gTextTexture, TextureSpriteSheets);

			bool gameover = false;

			while (!gameover)
			{
				gameover = game.Run();
			}

			for (;;);
		}
	}

	//Free resources and close SDL
	close();

	return 0;
}

bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER | SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC) < 0)
	//if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		{
			printf("Warning: Linear texture filtering not enabled!");
		}

		//Check for joysticks
		if (SDL_NumJoysticks() < 1)
		{
			printf("Warning: No joysticks connected!\n");
		}
		else
		{
			//Load joystick
			gGameController = SDL_JoystickOpen(0);
			if (gGameController == NULL)
			{
				printf("Warning: Unable to open game controller! SDL Error: %s\n", SDL_GetError());
			}
			else
			{
				// get controller haptic device
				gControllerHaptic = SDL_HapticOpenFromJoystick(gGameController);
				if (gControllerHaptic == NULL)
				{
					printf("Warning: Controller does not support haptics! SDL Error: %s\n", SDL_GetError());
				}
				else
				{
					//Get initialize rumble
					if (SDL_HapticRumbleInit(gControllerHaptic) < 0)
					{
						printf("Warning: Unable to initialize rumble! SDL Error: %s\n", SDL_GetError());
					}
				}
			}
		}

		//Create window
		gWindow = SDL_CreateWindow("SDL-Berzerk   Esc: quit", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			WindowIcon();
			//Create vsynced renderer for window
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED /*| SDL_RENDERER_PRESENTVSYNC */);
			if (gRenderer == NULL)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags))
				{
					printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
					success = false;
				}

				// init SDL_ttf
				if (TTF_Init() == -1)
				{
					printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
					success = false;
				}
			}

			SDL_ShowCursor(SDL_DISABLE);
		}
	}

	return success;
}

bool loadMedia()
{
	//Loading success flag
	bool success = true;

	//Open the font
	gFont = TTF_OpenFont("FreeSansBold.ttf", 24);
	//gFont = TTF_OpenFont("kenvector_future.ttf", 24);
	if (gFont == NULL)
	{
		printf("Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError());
		success = false;
	}
	else
	{
		//Render text
		SDL_Color textColor = { 0, 0, 0 };
		gTextTexture.gRenderer = gRenderer;
		if (!gTextTexture.loadFromRenderedText("The quick brown fox jumps over the lazy dog", textColor))
		{
			printf("Failed to render text texture!\n");
			success = false;
		}

		loadSprites();
	}

	return success;
}

struct SpriteSheet
{
	OBJ_TYPE obj_type;
	std::string file;
};

static SpriteSheet sprites[] = {
	{ OBJ_TYPE::PLAYER, "player2.png" },
	{ OBJ_TYPE::ROBOT, "robot2.png" },
	{ OBJ_TYPE::ROBOT_EXPLODE, "robotexplode.png" },
	{ OBJ_TYPE::BULLET, "bullets2.png" },
	{ OBJ_TYPE::LIFE, "lives.png" },
	{ OBJ_TYPE::OTTO, "otto2.png" }
};

bool loadSprites()
{


	for (int i = 0; i < sizeof(sprites) / sizeof(SpriteSheet); i++)
	{
		OBJ_TYPE obj = sprites[i].obj_type;
		std::string file = sprites[i].file;

		Texture * ptexture = new Texture();
		ptexture->gRenderer = gRenderer;

		if (!ptexture->loadFromFile(file))
		{
			printf("Failed to load %s texture!\n", file);
			return false;
		}

		TextureSpriteSheets[obj] = ptexture;
	}

	return true;
}

void close()
{
	//Free loaded images
	gTextTexture.free();

	//Close game controller with haptics
	SDL_HapticClose(gControllerHaptic);
	gControllerHaptic = NULL;
	SDL_JoystickClose(gGameController);
	gGameController = NULL;

	//Free global font
	TTF_CloseFont(gFont);
	gFont = NULL;

	// free sprite sheets
	for (int i = 0; i < sizeof(sprites) / sizeof(SpriteSheet); i++)
	{
		TextureSpriteSheets[sprites[i].obj_type]->free();
	}

	//Destroy window	
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

SDL_Surface *iconsurface;     // Declare an SDL_Surface to be filled in with pixel data from an image file
Uint16 pixels[16 * 16] = {  // ...or with raw pixel data:
	0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff,
	0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff,
	0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff,
	0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff,
	0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff,
	0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff,
	0x0fff, 0x0aab, 0x0789, 0x0bcc, 0x0eee, 0x09aa, 0x099a, 0x0ddd,
	0x0fff, 0x0eee, 0x0899, 0x0fff, 0x0fff, 0x1fff, 0x0dde, 0x0dee,
	0x0fff, 0xabbc, 0xf779, 0x8cdd, 0x3fff, 0x9bbc, 0xaaab, 0x6fff,
	0x0fff, 0x3fff, 0xbaab, 0x0fff, 0x0fff, 0x6689, 0x6fff, 0x0dee,
	0xe678, 0xf134, 0x8abb, 0xf235, 0xf678, 0xf013, 0xf568, 0xf001,
	0xd889, 0x7abc, 0xf001, 0x0fff, 0x0fff, 0x0bcc, 0x9124, 0x5fff,
	0xf124, 0xf356, 0x3eee, 0x0fff, 0x7bbc, 0xf124, 0x0789, 0x2fff,
	0xf002, 0xd789, 0xf024, 0x0fff, 0x0fff, 0x0002, 0x0134, 0xd79a,
	0x1fff, 0xf023, 0xf000, 0xf124, 0xc99a, 0xf024, 0x0567, 0x0fff,
	0xf002, 0xe678, 0xf013, 0x0fff, 0x0ddd, 0x0fff, 0x0fff, 0xb689,
	0x8abb, 0x0fff, 0x0fff, 0xf001, 0xf235, 0xf013, 0x0fff, 0xd789,
	0xf002, 0x9899, 0xf001, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0xe789,
	0xf023, 0xf000, 0xf001, 0xe456, 0x8bcc, 0xf013, 0xf002, 0xf012,
	0x1767, 0x5aaa, 0xf013, 0xf001, 0xf000, 0x0fff, 0x7fff, 0xf124,
	0x0fff, 0x089a, 0x0578, 0x0fff, 0x089a, 0x0013, 0x0245, 0x0eff,
	0x0223, 0x0dde, 0x0135, 0x0789, 0x0ddd, 0xbbbc, 0xf346, 0x0467,
	0x0fff, 0x4eee, 0x3ddd, 0x0edd, 0x0dee, 0x0fff, 0x0fff, 0x0dee,
	0x0def, 0x08ab, 0x0fff, 0x7fff, 0xfabc, 0xf356, 0x0457, 0x0467,
	0x0fff, 0x0bcd, 0x4bde, 0x9bcc, 0x8dee, 0x8eff, 0x8fff, 0x9fff,
	0xadee, 0xeccd, 0xf689, 0xc357, 0x2356, 0x0356, 0x0467, 0x0467,
	0x0fff, 0x0ccd, 0x0bdd, 0x0cdd, 0x0aaa, 0x2234, 0x4135, 0x4346,
	0x5356, 0x2246, 0x0346, 0x0356, 0x0467, 0x0356, 0x0467, 0x0467,
	0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff,
	0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff,
	0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff,
	0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff
};

void WindowIcon2()
{
	iconsurface = SDL_CreateRGBSurfaceFrom(pixels, 16, 16, 16, 16 * 2, 0x0f00, 0x00f0, 0x000f, 0xf000);

	// The icon is attached to the window pointer
	SDL_SetWindowIcon(gWindow, iconsurface);

	// ...and the surface containing the icon pixel data is no longer required.
	SDL_FreeSurface(iconsurface);
}


void WindowIcon()
{

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load("icon.png");
	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", "ICON", IMG_GetError());
	}

	//Color key image
	//SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0x00, 0xFF, 0x00));

	// The icon is attached to the window pointer
	SDL_SetWindowIcon(gWindow, loadedSurface);

	// ...and the surface containing the icon pixel data is no longer required.
	SDL_FreeSurface(loadedSurface);
}
