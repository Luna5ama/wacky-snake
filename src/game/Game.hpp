#pragma once

#include "World.hpp"
#include "Snake.hpp"

enum class State {
	Waiting,
	Playing,
	Overing,
};

class Game {
public:
	Snake player;
	World world;
	long double timeElapsed = 0.0;

	State state = State::Waiting;


	Game() :
		player(), world() {}

	void placeFood(int n = 1) {
		for (int i = 0; i < n; ++i) {
			world.placeFood(player);
		}
	}

	void tick(double dt) {

		switch (state)
		{
		case State::Waiting:
			// give 1 second so stuff has a chance to load
			timeElapsed += dt;
			
			if (timeElapsed >= 1.0) {
				state = State::Playing;
				timeElapsed = 0.0f;
			}

			break;

		case State::Playing:
			timeElapsed += dt;
			// if they didn't lose
			if (player.tick(dt, world) != LoseCode::None)
				state = State::Overing;
			break;

		case State::Overing:
			// game over :(
			break;
		default:
			break;
		}
	}

	double getScore() {
		return timeElapsed + player.foodsEaten;
	}
};