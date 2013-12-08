#include "zombiegame.h"
#include "inputkeyboard.h"
#include "weapon.h"
#include "movingobject.h"
#include "building.h"
#include "auxiliary.h"
#include "load.h"
#include "settings.h"
#include "task.h"
#include "car.h"

// Graphic.
#include "mapdraw.h"
#include "buildingdraw.h"
#include "unitanimation.h"
#include "caranimation.h"

// External.
#include <mw/exception.h>

// Stl.
#include <cmath>
#include <sstream>

namespace zombie {

	ZombieGame::ZombieGame(const GameData& gameData) : engine_(this), gameData_(gameData) {
		// Set windows size.
		updateSize(gameData.getWidth(), gameData.getHeight());

		keyboard1_ = DevicePtr(new InputKeyboard(SDLK_UP, SDLK_DOWN, SDLK_LEFT,
			SDLK_RIGHT, SDLK_SPACE, SDLK_r, SDLK_LSHIFT, SDLK_e));

		scale_ = 1.0;

		taskManager_.add(new MapDraw(-50, 50, -50, 50), GraphicLevel::GROUND_LEVEL);

		innerSpawnRadius_ = 10.f;
		outerSpawnRadius_ = 40.f;

		gameData_.humanPlayer([&](State state, UnitProperties uP, const Animation& animation) {
			UnitAnimation* unitAnimation = new UnitAnimation(state, uP.radius_, animation);
			taskManager_.add(unitAnimation, GraphicLevel::UNIT_LEVEL);

			auto callback = std::bind(&UnitAnimation::updateData, unitAnimation, std::placeholders::_1, std::placeholders::_2);
			engine_.setHuman(keyboard1_, state,
				uP.mass_, uP.radius_, uP.life_, uP.walkingSpeed_,
				uP.runningSpeed_, Weapon(55, 0.2f, 8, 12), callback);

			viewPosition_ = state.position_;
		});

		gameData_.iterateCars([&](State state, CarProperties cP, const mw::Sprite& sprite) {
			CarAnimation* carAnimation = new CarAnimation(state, cP.width_, cP.length_, sprite);
			taskManager_.add(carAnimation, GraphicLevel::UNIT_LEVEL);

			auto callback = std::bind(&CarAnimation::updateData, carAnimation, std::placeholders::_1, std::placeholders::_2);
			engine_.addCar(state, cP.mass_, cP.life_,
				cP.width_, cP.length_, callback);
		});

		gameData_.iterateUnits([&](State state, UnitProperties uP, const Animation& animation) {
			UnitAnimation* unitAnimation = new UnitAnimation(state, uP.radius_, animation);
			taskManager_.add(unitAnimation, GraphicLevel::UNIT_LEVEL);

			auto callback = std::bind(&UnitAnimation::updateData, unitAnimation, std::placeholders::_1, std::placeholders::_2);
			engine_.addAi(state, uP.mass_, uP.radius_,
				uP.life_, uP.walkingSpeed_, uP.
				runningSpeed_, true, Weapon(35, 0.5f, 1, 10000), callback);
		});

		gameData_.iterateBuildings([&](BuildingProperties bP) {
			engine_.addBuilding(bP.points_);
			taskManager_.add(new BuildingDraw(bP.points_), GraphicLevel::BUILDING_LEVEL);
		});
	}

	ZombieGame::~ZombieGame() {
	}

	// Starts the game.
	void ZombieGame::startGame() {
		engine_.start();
	}

	void ZombieGame::update(float deltaTime) {
		engine_.update(deltaTime);

		// Draw map centered around first human player.
		glPushMatrix();

		glTranslated(0.5, 0.5, 0);
		glScale2f(1.f / 50); // Is to fit the box drawn where x=[0,1] and y=[0,1].
		glScale2f(scale_); // Is to fit the box drawn where x=[0,1] and y=[0,1].
		glTranslate2f(-viewPosition_);

		// Game is started?
		if (engine_.isStarted()) {
			taskManager_.update(deltaTime);
		} else {
			taskManager_.update(0.0);
		}

		glPopMatrix();
	}

	void ZombieGame::humanPosition(float x, float y) {
		viewPosition_ += 0.1f * (Position(x, y) - viewPosition_);
	}

	void ZombieGame::zoom(float scale) {
		scale_ *= scale;
	}

	void ZombieGame::updateSize(int width, int height) {
		Task::width = width;
		Task::height = height;
	}

	void ZombieGame::eventUpdate(const SDL_Event& windowEvent) {
		// Update human input.
		keyboard1_->eventUpdate(windowEvent);
	}

} // Namespace zombie.
