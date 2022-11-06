#include "Game.hpp"

Game::Game() : player(), world(), timeElapsed(0.0), state(State::Waiting) {}

void Game::placeFood(int n) {
	for (int i = 0; i < n; ++i) {
		this->world.placeFood(this->player);
	}
}

void Game::tick(double dt) {
	switch (this->state) {
		case State::Waiting:
			// give 1 second so stuff has a chance to load
			this->timeElapsed += dt;
			if (this->timeElapsed >= 1.0) {
				this->state = State::Playing;
				this->timeElapsed = 0.0f;
			}
			break;
		case State::Playing:
			this->timeElapsed += dt;
			// if they didn't lose
			if (this->player.tick(dt, this->world) != LoseCode::None)
				this->state = State::Overing;
			break;
		case State::Overing:
			// game over :(
			break;
		default:
			break;
	}
}

int Game::getScore() {
	return (int) timeElapsed + player.foodsEaten;
}