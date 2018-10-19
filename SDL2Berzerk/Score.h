// Score.h

#ifndef __SCORE__
#define __SCORE__
#include <SDL.h>
#include "SDL2Berzerk.h"
#include "AnimateObj.h"
#include "Texture.h"

class Score : public AnimateObj
{
public:
	static const int SCORE_WIDTH = 20;  // 12;
	static const int SCORE_HEIGHT = 24; // 16;

	Score(Texture *, SDL_Color);
	~Score();

	void render();

	void robotkill() { total += 50; }
	void bonus(int kills) { total += kills * 10; }

	int GameScore() { return total; }

private:
	int total = 0;
};
#endif
