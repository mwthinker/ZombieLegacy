#include "zombiegame.h"
#include "inputkeyboard.h"
#include "weapon.h"
#include "movingobject.h"
#include "building.h"
#include "auxiliary.h"

#include <cmath>

namespace zombie {

	void loadMap(tinyxml2::XMLHandle xml) {

	}

	// Returns a random postion between the defined outer and inner circle centered in position.
	Position generatePosition(Position position, float innerRadius, float outerRadius) {
		return position + (innerRadius + (outerRadius - innerRadius) * random()) * Position(std::cosf(random()*2.f*3.14f), std::sinf(random()*2.f*3.14f));
	}

	const Position ORIGO(0,0);

	ZombieGame::ZombieGame(int width, int height, tinyxml2::XMLHandle xml) : engine_(width, height) {
		// Set windows size.
		updateSize(width,height);
		
		keyboard1_ = DevicePtr(new InputKeyboard(SDLK_UP, SDLK_DOWN, SDLK_LEFT, 
			SDLK_RIGHT, SDLK_SPACE, SDLK_r,SDLK_LSHIFT,SDLK_e));

		engine_.addGrassGround(-50, 50, -50, 50);

		innerSpawnRadius_ = 10.f;
		outerSpawnRadius_ = 40.f;
		
		Position position = generatePosition(ORIGO, 0, 50);
		engine_.addHuman(keyboard1_,position.x, position.y, 0.3f, Weapon(55,0.2f,8,12));
		engine_.addEventListener(std::bind(&ZombieGame::handleGameEvent, this, std::placeholders::_1));

		// Add cars.
		for (int i = 0; i < 10; ++i) {
			Position spawn = generatePosition(ORIGO, 0, 50);
			engine_.addCar(spawn.x, spawn.y);
		}

		// Add zombies.
		for (int i = 0; i < 1; ++i) {
			Position spawn = generatePosition(ORIGO, 0, 50);
			engine_.addAi(spawn.x, spawn.y, 0.3f, Weapon(35,0.5f,1,10000), true);
		}

		// Add survivors.
		for (int i = 0; i < 10; ++i) {
			Position spawn = generatePosition(ORIGO, 0, 50);
			engine_.addAi(spawn.x, spawn.y, 0.f, Weapon(35,0.5,8,120), false);
		}

		// Add weapons.
		for (int i = 0; i < 10; ++i) {
			Position spawn = generatePosition(ORIGO, 0, 50);
			Weapon weapon;
			engine_.addWeapon(spawn.x, spawn.y, weapon);
		}
	}

	ZombieGame::~ZombieGame() {
	}

	void ZombieGame::handleGameEvent(const GameEvent& gameEvent) {
		if (const UnitDie* unitDie = dynamic_cast<const UnitDie*>(&gameEvent)) {
			Position spawn = generatePosition(engine_.getMainUnitPostion(), innerSpawnRadius_, outerSpawnRadius_);
			engine_.addAi(spawn.x, spawn.y, 0.3f, Weapon(35,0.5f,1,10000), true);
		}
	}

	void ZombieGame::handleRemoval(bool& remove, MovingObject* mOb) {
		bool outside = mOb->getPosition().LengthSquared() > outerSpawnRadius_ * outerSpawnRadius_;
		remove = outside;
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
		// Update human input.
		keyboard1_->eventUpdate(windowEvent);
	}

} // Namespace zombie.
