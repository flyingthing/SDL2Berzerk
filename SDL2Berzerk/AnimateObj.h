// AnimateObj.h

#ifndef __ANIMATEOBJ__
#define __ANIMATEOBJ__
#include <SDL.h>
#include "SDL2Berzerk.h"
#include <string>
#include <map>
#include <list>
#include <vector>


typedef std::map<std::string, std::vector<int>> Dict;

class AnimateObj
{
	public:

		AnimateObj(OBJ_TYPE, Texture *, SDL_Color, int, int, void (*)(void *));
		~AnimateObj();

		void addPattern(std::string, std::vector<int>);
		void setPattern(std::string);
		std::string getPattern();
		//int xOffSet();
		int yOffSet();
		void setPos(int, int);
		void setFrame(int);
		void setWidthHeight(int, int);
		SDL_Color getColor();
	
		virtual int xOffSet();
		virtual void setColor(SDL_Color);
		virtual void move() {};
		virtual void move(int, int) {};
		virtual void render();

		// object collision box
		SDL_Rect mCollider;

		// x and y offsets of the object
		int mPosX, mPosY;

		void(*cbFrame)(void *) = NULL;

		OBJ_TYPE obj_type = OBJ_TYPE::UNKNOWN;

	protected:
		void update();

		int width;
		int height;

		SDL_Color color;

		Dict d;

		std::string currentPattern = "";

		// current animation frame
		int frame = 0;

		int vframe = 10;

		Texture * gSpriteTexture = NULL;
};
#endif

