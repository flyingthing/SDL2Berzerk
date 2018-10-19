#include "AnimateObj.h"


AnimateObj::AnimateObj(OBJ_TYPE obj_type, Texture * pTexture, SDL_Color color, int width, int height, void (*callback)(void *))
{
	this->width = width;
	this->height = height;

	this->cbFrame = callback;

	mCollider.w = width;
	mCollider.h = height;

	//Initialize the offsets
	mPosX = 0;
	mPosY = 0;

	this->obj_type = obj_type;
	this->gSpriteTexture = pTexture;
	this->setColor(color);
}

AnimateObj::~AnimateObj()
{
	d.clear();
}

void AnimateObj::addPattern(std::string pattern, std::vector<int> frames)
{
	d[pattern] = frames;
}

void AnimateObj::setPattern(std::string pattern)
{
	currentPattern = pattern;
	frame = 0;
}

std::string AnimateObj::getPattern()
{
	return currentPattern;
}

void AnimateObj::setPos(int x, int y)
{
	mCollider.x = mPosX = x;
	mCollider.y = mPosY = y;
}

void AnimateObj::setFrame(int frame)
{
	this->frame = frame;
}

void AnimateObj::setWidthHeight(int w, int h)
{
	mCollider.w = width = w;
	mCollider.h = height = h;
}

int AnimateObj::xOffSet()
{
	return d[currentPattern].at(frame);
}

int AnimateObj::yOffSet()
{
	return 0;
}

void AnimateObj::setColor(SDL_Color color)
{
	// modulate color(r,g,b)
	this->color = color;

	if (gSpriteTexture != NULL)
	{
		gSpriteTexture->setColor(color.r, color.g, color.b);
	}
}

SDL_Color AnimateObj::getColor()
{
	return color;
}


void AnimateObj::update()
{
	// goto next frame
	frame++;

	// cycle animation
	if (frame >= d[currentPattern].size())
	{
		if (cbFrame != NULL)
		{
			cbFrame((void *)this);
		}

		frame = 0;
	}
}


void AnimateObj::render()
{
	// render current frame
	SDL_Rect currentClip;
	currentClip.x = xOffSet() * width;
	currentClip.y = 0;
	currentClip.w = width;
	currentClip.h = height;

	// show object on scrren
	gSpriteTexture->render(mPosX, mPosY, &currentClip);

	vframe--;

	if (vframe == 0)
	{
		update();
		vframe = 5;
	}
}
