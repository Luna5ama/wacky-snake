#pragma once

#include "GameLogics.h"
#include "GLObjects.hpp"

class Game {
	Snake player;
	World world;
	long double timeElapsed = 0.0;

	bool playing = true;

public:

	Game() :
		player(), world() {}

	void tick(double dt) {
		if (playing) {
			timeElapsed += dt;

			playing = player.tick(dt, world);
		}
	}
};