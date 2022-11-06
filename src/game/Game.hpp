#pragma once

#include "World.hpp"
#include "Snake.hpp"

class Game {
	Snake player;
	World world;
	long double timeElapsed = 0.0;

	bool playing = true;

public:

	Game() :
		player(), world() {}

	void placeFood(int n = 1) {
		for (int i = 0; i < n; ++i) {
			world.placeFood(player);
		}
	}

	void tick(double dt) {
		if (playing) {
			timeElapsed += dt;

			// if they didn't lose
			playing = player.tick(dt, world) == LoseCode::None;
		}
	}

	double getScore() {
		return timeElapsed + player.foodsEaten;
	}
};