#pragma once
#include "SDL.h"
#include <stdio.h>
#include "SerialInterface.h"
#include "SDL_image.h"
#include <vector>
#include "Enemy.h"
#include "Boss.h"
#include "Globals.h"

class Game
{
public:
	Game();
	~Game();

	int extraEnemies = 0;

	int extraBosses = 0;

	int score = 0;

	int lives = Globals::lives;

	float enemySpeed = 0.5;

	float quickEnemySpeed = 2.0;

	float bossSpeed = 0.5;

	bool init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen);

	void handleEvents();

	void update();

	void render();

	void clean();

	bool running()
	{
		return isRunning;
	};

	bool gameOver = false;

	static SDL_Renderer *renderer;
	static SDL_Event event;

	int lastRot1 = 5000;
	int thisRot1;

private:

	int cnt = 0;

	SDL_Window * mainWindow;


	bool isRunning;

	bool keyHasFired = false;

	SerialInterface* serialInterface; //should really be put after SDL runs

	std::vector<Enemy*> enemylist;

	std::vector<Boss*> bosslist;

	std::vector<Object*> heartlist;

	std::vector<Object*> heartlist2;
};