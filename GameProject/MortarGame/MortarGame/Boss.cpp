#include "Boss.h"
#include "Object.h"
#include "GraphicsManager.h"



Boss::Boss()
{
	xpos = 0;
	ypos = 0;
	objTexture = nullptr;

}

Boss::Boss(const char * texturesheet, float x, float y) 
{
	objTexture = GraphicsManager::LoadImage(texturesheet);

	xpos = x;
	ypos = y;
}


void Boss::Update()
{
	//Object::Update();

	sourceRect.h = 256;
	sourceRect.w = 256;
	sourceRect.x = 0;
	sourceRect.y = 0;

	destRect.x = xpos;
	destRect.y = ypos;
	destRect.h = sourceRect.h;
	destRect.w = sourceRect.w;
}

//function to move enemy down the screen
void Boss::MoveDown(float speed)
{
	Boss::Update();

	ypos = ypos + speed;
}


//function to respawn and stop the enemy
void Boss::StopMoving()
{
	Object::Update();

	ypos = -256;
}

void Boss::Render()
{
	//SDL_SetTextureColorMod(objTexture, 255, 0, 0);
	SDL_RenderCopy(Game::renderer, objTexture, &sourceRect, &destRect);
}

Boss::~Boss()
{
}
