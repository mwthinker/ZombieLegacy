#include "zombiegame.h"
#include "inputkeyboard.h"

namespace zombie {

	ZombieGame::ZombieGame(int width, int height) : engine_(width, height) {
		// Set windows size.
		updateSize(width,height);
		
		keyboard1_ = DevicePtr(new InputKeyboard(SDLK_UP,SDLK_DOWN,SDLK_LEFT,SDLK_RIGHT,SDLK_SPACE,SDLK_r,SDLK_LSHIFT,SDLK_e));
		keyboard2_ = DevicePtr(new InputKeyboard(SDLK_w,SDLK_s,SDLK_a,SDLK_d,SDLK_f,SDLK_g,SDLK_h,SDLK_z));

		map_ = loadMapInfo("housesFME.mif","roadsFME.mif", 1);
		engine_.init(map_, keyboard1_, keyboard2_);
	}

	ZombieGame::~ZombieGame() {
	}

	// Starts the game.
	void ZombieGame::startGame() {
		engine_.start();
	}

	void ZombieGame::update(float deltaTime) {
		engine_.update(deltaTime);
	}

	void ZombieGame::zoom(double scale) {
		engine_.zoom(scale);
	}

	void ZombieGame::updateSize(int width, int height) {
		engine_.updateSize(width, height);
	}

	void ZombieGame::eventUpdate(const SDL_Event& windowEvent) {
		// Game is started?
		// Update all human input.
		keyboard1_->eventUpdate(windowEvent);
		keyboard2_->eventUpdate(windowEvent);
	}

} // Namespace zombie.
