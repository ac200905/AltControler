#include "stdafx.h"
#include "Game.h"
#include "GraphicsManager.h"
#include "Object.h"
#include "Background.h"
#include "Player.h"
#include "Enemy.h"
#include "Boss.h"
#include <iostream>
#include <time.h>       /* time */
#include "Globals.h"
#include <SDL_ttf.h>

using std::cout;
using std::endl;

Player* player;
Enemy* enemy;
Enemy* quickEnemy;
Boss* boss;
Object* explosion;
Object* restart;
Background* background;
Object* redCrosshair;
Object* heart;


SDL_Renderer* Game::renderer = nullptr;
SDL_Event Game::event;

int numberofenemies = 2;
int numberofbosses = 0;
int bossHealth = Globals::bossHealth;
int xValue = 0;
int xValue2 = 0;




/*
* Constructor - not used in this case
*
*/
Game::Game()
{
	serialInterface = new SerialInterface();

}

/*
Decontructor - also not used in this case
*/
Game::~Game()
{
}

/*
* init - used to initialise and setup SDL
* Return true if everything is successful
*/
bool Game::init(const char * title, int xpos, int ypos, int width, int height, bool fullscreen)
{
	srand(time(NULL));
	int flags = 0;
	if (fullscreen)
	{
		flags = SDL_WINDOW_FULLSCREEN;
	}

	// initialise SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) == 0)
	{
		cout << "SDL init success \n";

		// Create a window
		mainWindow = SDL_CreateWindow(title, xpos, ypos, width, height, flags);

		// if window succesful..
		if (mainWindow != 0)
		{
			cout << "Window creation success \n";

			// create renderer
			renderer = SDL_CreateRenderer(mainWindow, -1, 0);

			// if renderer successful...
			if (renderer != 0)
			{
				cout << "Renderer creation success \n";
				SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
			}
			else
			{
				cout << "renderer failed \n";
				return false;
			}
		}
		else
		{
			cout << "window failed \n";
			return false;
		}


	}
	else
	{
		cout << "SDL fail \n";
		return false;
	}

	isRunning = true;
	cout << "SDL init success \n";

	//instantiate serial here

	player = new Player("Assets/Crosshair.png", Globals::screenWidth / 2 -32, Globals::screenHeight / 2 -32);
	player->serialInterface = serialInterface;
	explosion = new Object("Assets/Explosion2.png", player->xpos, player->ypos);
	restart = new Object("Assets/RestartButton.png", 0, 0);
	redCrosshair = new Object("Assets/CrosshairRed.png", player->xpos, player->ypos);

	quickEnemy = new Enemy("Assets/Boss.png", rand() % 736 + 1, rand() % 40 - 1500);

	background = new Background("Assets/Road.png", 0, 0);

	for (int i = 0; i < lives; i++)
	{
		heartlist.push_back(new Object("Assets/Heart.png", 0 - xValue, Globals::screenHeight - 64));
		xValue = xValue - 64;
	}

	for (int i = 0; i < lives; i++)
	{
		heartlist2.push_back(new Object("Assets/HeartEmpty.png", 0 - xValue2, Globals::screenHeight - 64));
		xValue2 = xValue2 - 64;
	}

	for (int i = 0; i < numberofenemies; i++)
	{
		enemylist.push_back(new Enemy("Assets/Enemy.png", rand() % 736 + 1, rand() % 40 -104));
	}

	for (int i = 0; i < numberofbosses; i++)
	{
		bosslist.push_back(new Boss("Assets/Boss2.png", 277, -256));
	}

	return true;
}


/*
* handleEvents - Poll Events and uses switch case to process specific events
*
*/
void Game::handleEvents()
{
	SDL_Event event;

	if (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			isRunning = false;
			break;
		case SDL_MOUSEBUTTONDOWN:
			break;
		case SDL_KEYDOWN:   // Key pressed
			switch (event.key.keysym.sym)
			{
			case SDLK_LEFT:
				player->xpos = player->xpos - 10;
				break;
			case SDLK_RIGHT:
				player->xpos = player->xpos + 10;
				break;
			case SDLK_UP:
				player->ypos = player->ypos - 10;
				break;
			case SDLK_DOWN:
				player->ypos = player->ypos + 10;
				break;
			case SDLK_SPACE:
				keyHasFired = true;

 				cout << "should fire" << endl;
				break;

			}
			
		default:
			break;
		}

	}
}


/*
* update - Process all variables !in charge of anything to do with rendering
*
*/
void Game::update()
{
	//update player positions
	player->Update();

	explosion->Update();
	redCrosshair->Update();

	background->Update();

	restart->Update();

	explosion->xpos = player->xpos;
	explosion->ypos = player->ypos;

	redCrosshair->xpos = player->xpos;
	redCrosshair->ypos = player->ypos;

	quickEnemy->Update();

	//if player fires the cannon while intersecting with an enemy, reset that enemy's
	//position to the top of the screen and increase player score
	if (player->hasFired() || keyHasFired == true)
	{
		
		cout << "Fired!!!" << endl;
		for (Enemy* currentEnemy : enemylist)
		{
			if (currentEnemy->isPointInside(player->xpos + 32, player->ypos + 32))
			{
				currentEnemy->ypos = -64;
				currentEnemy->xpos = rand() % 736 + 1;
				score++;
				cout << "Score: " << score << endl;
			}
		}
		
		if (quickEnemy->isPointInside(player->xpos + 32, player->ypos + 32))
		{
			quickEnemy->xpos = rand() % 736 + 1;
			quickEnemy->ypos = -2000;
			score = score + 2;
			cout << "Score: " << score << endl;
		}

		for (Boss* currentBoss : bosslist)
		{
			if (currentBoss->isPointInside(player->xpos + 32, player->ypos + 32))
			{
				
				//reduce health of boss
				if (bossHealth > 0)
				{
					bossHealth--;
				}
				if (bossHealth <= 0)
				{
					bossHealth = Globals::bossHealth;
					currentBoss->ypos = -600;
					currentBoss->xpos = Globals::screenWidth/2 - 256/2;
					if (bosslist.size() == 1)
					{
						bosslist.push_back(new Boss("Assets/Boss2.png", Globals::screenWidth / 2 - 256 / 2, -600));
					}
					score = score + 4;
					cout << "Score: " << score << endl;
				}
				
			}
		}

		if (restart->isPointInside(player->xpos + 32, player->ypos + 32))
		{
			gameOver = true;

		}
	}

	
	if (player->xpos < -64)
	{
		player->xpos = Globals::screenWidth;
	}

	if (player->xpos > Globals::screenWidth)
	{
		player->xpos = -64;
	}

	if (player->ypos < -64)
	{
		player->ypos = Globals::screenHeight;
	}

	if (player->ypos > Globals::screenHeight)
	{
		player->ypos = -64;
	}

	if (heartlist.size() > lives)
	{
		heartlist.erase(heartlist.begin(), heartlist.begin() + (1));
	}

	if (gameOver == true)
	{
		lives = Globals::lives;
		score = 0;
		cout << "New Game Started" << endl;
		cout << "Score: " << score << endl;
		if (enemylist.size() > numberofenemies)
		{
			enemylist.erase(enemylist.begin(),enemylist.begin() + extraEnemies);
		}

		if (bosslist.size() > numberofbosses)
		{
			bosslist.erase(bosslist.begin(), bosslist.begin() + extraBosses);
		}

		enemySpeed = Globals::enemySpeed1;
		bossSpeed = 0.5;
		extraEnemies = 0;
		extraBosses = 0;
		xValue = 0;

		for (auto iter = heartlist.begin(); iter != heartlist.end(); )
		{
			if ((*iter))
			{
				delete (*iter);
				iter = heartlist.erase(iter);
			}

			else
			{
				iter++;
			}
		}


		
		for (int i = 0; i < lives; i++)
		{
			heartlist.push_back(new Object("Assets/Heart.png", 0 - xValue, Globals::screenHeight - 64));
			xValue = xValue - 64;
		}

		quickEnemy = new Enemy("Assets/Boss.png", rand() % 736 + 1, rand() % 40 - 1200);
			
		gameOver = false;

		for (Enemy* currentEnemy : enemylist)
		{
			currentEnemy->xpos = rand() % 736 + 1;
			currentEnemy->ypos = rand() % 40 - 104;
		}

		for (Boss* currentBoss : bosslist)
		{
			currentBoss->xpos = rand() % 736 + 1;
			currentBoss->ypos = rand() % 40 - 104;
		}

		quickEnemy->xpos = rand() % 736 + 1;
		quickEnemy->ypos = rand() % 40 - 1200;
	}

	//change the enemies speed depending on how high the players score is
	if (score < 3)
	{
		enemySpeed = Globals::enemySpeed1;
		quickEnemySpeed = Globals::quickEnemySpeed1;
	}

	if ((score >= 3) && (score < 6))
	{
		enemySpeed = Globals::enemySpeed2;

		if (enemylist.size() <= 2)
		{
			enemylist.push_back(new Enemy("Assets/Enemy.png", rand() % 736 + 1, rand() % 40 - 104));
			
			extraEnemies++;
		}
	}

	if ((score >= 6) && (score < 9))
	{
		enemySpeed = Globals::enemySpeed3;

		if (enemylist.size() <= 3)
		{
			enemylist.push_back(new Enemy("Assets/Enemy.png", rand() % 736 + 1, rand() % 40 - 104));
			bosslist.push_back(new Boss("Assets/Boss3.png", Globals::screenWidth / 2 - 256 / 2, -256));
			bosslist.push_back(new Boss("Assets/Boss2.png", Globals::screenWidth / 2 - 256 / 2, -256));
			extraEnemies++;
			extraBosses++;
			extraBosses++;
		}
	}

	if ((score >= 9) && (score < 12))
	{
		enemySpeed = Globals::enemySpeed4;
	}

	if ((score >= 12) && (score < 15))
	{
		enemySpeed = Globals::enemySpeed5;
		quickEnemySpeed = Globals::quickEnemySpeed2;

		if (enemylist.size() <= 4)
		{
			enemylist.push_back(new Enemy("Assets/Enemy.png", rand() % 736 + 1, rand() % 40 - 104));
			extraEnemies++;
		}
	}

	if ((score >= 15) && (score < 18))
	{
		enemySpeed = Globals::enemySpeed6;
	}

	if (score >= 18)
	{
		enemySpeed = Globals::enemySpeed7;

		if (enemylist.size() <= 5)
		{
			enemylist.push_back(new Enemy("Assets/Enemy.png", rand() % 736 + 1, rand() % 40 - 104));
			extraEnemies++;
		}
	}

	for (Object* currentHeart : heartlist)
	{
		currentHeart->Update();
	}

	for (Object* currentHeart : heartlist2)
	{
		currentHeart->Update();
	}

	quickEnemy->MoveDown(quickEnemySpeed);

	if (lives <= 0)
	{
		quickEnemy->StopMoving();
	}

	if (quickEnemy->ypos > Globals::screenHeight + 64)
	{
		quickEnemy->xpos = rand() % 736 + 1;
		quickEnemy->ypos = rand() % 40 - 1200;
		lives--;
		cout << "Lives left: " << lives - 1 << endl;

		//end of game, show final score
		if (lives <= 0)
		{
			cout << "Game Over!" << endl;
			cout << "Final Score: " << score << endl;
			score = 0;
			SDL_Delay(3000);
			gameOver = true;
		}
	}

	// Using the console to show the scoring and lives left.
	for (Enemy* currentEnemy : enemylist)
	{
		currentEnemy->MoveDown(enemySpeed);
		
		//at end of game stop all enemies
		if (lives <= 0)
		{
			currentEnemy->StopMoving();
		}

		//respawn the enemy at the top of the screen if the player misses it and
		//lower the players number of lives by one 
		if (currentEnemy->ypos > Globals::screenHeight + 64)
		{
			currentEnemy->xpos = rand() % 736 + 1;
			currentEnemy->ypos = rand() % 40 - 104;
			lives--;
			cout << "Lives left: " << lives - 1 << endl;

			//end of game, show final score
			if (lives <= 0)
			{
				cout << "Game Over!" << endl;
				cout << "Final Score: " << score << endl;
				score = 0;
				SDL_Delay(3000);
				gameOver = true;
			}
		}
	}

	for (Boss* currentBoss : bosslist)
	{
		currentBoss->MoveDown(bossSpeed);

		//at end of game stop all enemies
		if (lives <= 0)
		{
			currentBoss->StopMoving();
		}

		//respawn the enemy at the top of the screen if the player misses it and
		//lower the players number of lives by one 
		if (currentBoss->ypos > 896)
		{
			currentBoss->ypos = -600;
			currentBoss->xpos = Globals::screenWidth / 2 - 256 / 2;
			lives = lives - 2;
			cout << "Lives left: " << lives - 1 << endl;

			//end of game, show final score
			if (lives <= 0)
			{
				cout << "Game Over!" << endl;
				cout << "Final Score: " << score << endl;
				score = 0;
				SDL_Delay(3000);
				gameOver = true;
			}
		}
	}
}


void Game::render()
{
	// set background color
	//SDL_SetRenderDrawColor(renderer, 0, 204, 0, 255);

	

	// clear previous frame
	SDL_RenderClear(renderer);

	background->Render();

	restart->Render();


	if (bossHealth < 3 && bosslist.size() == 2)
	{
		bosslist.pop_back();
	}


	//render enemies from vector
	for (Enemy* currentEnemy : enemylist)
	{
		currentEnemy->Render();
	}

	quickEnemy->Render();

	for (Boss* currentBoss : bosslist)
	{
		currentBoss->Render();
	}

	for (Object* currentHeart : heartlist2)
	{
		currentHeart->Render();
	}

	for (Object* currentHeart : heartlist)
	{
		currentHeart->Render();
	}

	if (player->hasFired() || keyHasFired == true)
	{
		explosion->Render();
		keyHasFired = false;
	}


	//render player
	player->Render();

	for (Enemy* currentEnemy : enemylist)
	{
		if (currentEnemy->isPointInside(player->xpos + 32, player->ypos + 32))
		{
			redCrosshair->Render();
		}
	}

	for (Boss* currentBoss : bosslist)
	{
		if (currentBoss->isPointInside(player->xpos + 32, player->ypos + 32))
		{
			redCrosshair->Render();
		}
	}

	if (quickEnemy->isPointInside(player->xpos + 32, player->ypos + 32))
	{
		redCrosshair->Render();
	}

	// render new frame
	SDL_RenderPresent(renderer);
}


/*
* clean - Clean up SDL and close the port
*
*/
void Game::clean()
{
	// delete player and enemies
	delete player;
	delete explosion;
	delete restart;
	delete background;
	delete enemy;
	delete redCrosshair;
	delete boss;
	delete heart;
	delete quickEnemy;

	for (auto iter = enemylist.begin(); iter != enemylist.end(); )
	{
		if ((*iter))
		{
			delete (*iter);
			iter = enemylist.erase(iter);
		}

		else
		{
			iter++;
		}
	}

	for (auto iter = heartlist.begin(); iter != heartlist.end(); )
	{
		if ((*iter))
		{
			delete (*iter);
			iter = heartlist.erase(iter);
		}

		else
		{
			iter++;
		}
	}

	for (auto iter = bosslist.begin(); iter != bosslist.end(); )
	{
		if ((*iter))
		{
			delete (*iter);
			iter = bosslist.erase(iter);
		}

		else
		{
			iter++;
		}
	}

	heartlist.clear();
	enemylist.clear();
	bosslist.clear();
	serialInterface->close();
	cout << "Cleaning SDL \n";
	SDL_DestroyWindow(mainWindow);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
}