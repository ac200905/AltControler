#pragma once

#include "Object.h"

class Enemy :public Object
{
public:
	Enemy();
	Enemy(const char* texturesheet, float x, float y);


	void Update() override;

	void MoveDown(float speed);

	void StopMoving();

	~Enemy();
};

