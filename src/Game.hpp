#pragma once

#include "GameLogics.h"
#include "GLObjects.hpp"

class Game {
	Snake player;
	World world;
	long double timeElapsed = 0.0;

	bool gameOver = false;

public:

	void tick(double dt) {
		if (!gameOver) {
			timeElapsed += dt;

			gameOver = player.tick(dt, world);
		}
	}
};