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
	long double timeElapsed;
	State state;


	Game();
	void placeFood(int n = 1);
	void tick(double dt);
	int getScore();
};