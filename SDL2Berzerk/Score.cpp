#include "Score.h"
#include <sstream>
#include <iomanip>

Score::Score(Texture *pTexture, SDL_Color color) : AnimateObj(OBJ_TYPE::SCORE, pTexture, color, 0, 0, NULL)
{
}

Score::~Score()
{
}


void Score::render()
{
	//SDL_Color textColor = { 0x00, 0xFF, 0x00 };

	/*
	std::string score_text = std::to_string(GameScore());
	SDL_Surface* textSurface = TTF_RenderText_Solid(gFont, score_text.c_str(), textColor);
	SDL_Texture* text = SDL_CreateTextureFromSurface(gRenderer, textSurface);
	int text_width = textSurface->w;
	int text_height = textSurface->h;
	SDL_FreeSurface(textSurface);
	int y = SCREEN_HEIGHT - text_height - MAZE_YMAX - WALLTHICKNESS;
	SDL_Rect renderQuad = { 40, SCREEN_HEIGHT - 40, text_width, text_height };
	SDL_RenderCopy(gRenderer, text, NULL, &renderQuad);
	SDL_DestroyTexture(text);
	*/

	// set text to be rendered
	// in memory text stream
	std::stringstream scoreText;
	scoreText.str("");
	scoreText << std::setw(8) << GameScore();
	//timeText << "Average Frames Per Second " << avgFPS;
	//timeText << std::setprecision(4) << avgFPS;

	//Render text
	if (!gSpriteTexture->loadFromRenderedText(scoreText.str(), getColor()))
	{
		printf("Unable to render FPS texture!\n");
	}

	gSpriteTexture->render(MAZE_XMIN, (SCREEN_HEIGHT - WALLTHICKNESS - gSpriteTexture->getHeight()));

	//gTextTexture->loadFromRenderedText("123456", textColor);
	//gTextTexture->render(20, MAZE_YMAX + BORDERTHICKNESS + gTextTexture->getHeight() / 4);
}
