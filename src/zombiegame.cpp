#include "zombiegame.h"
#include "inputkeyboard.h"
#include "weapon.h"
#include "movingobject.h"
#include "building.h"
#include "auxiliary.h"
#include "load.h"
#include "settings.h"
#include "unit2d.h"
#include "car2d.h"
#include "weaponitem2d.h"
#include "building2d.h"
#include "terrain2d.h"

// External.
#include <mw/exception.h>

// Stl.
#include <cmath>
#include <sstream>

namespace zombie {

	namespace {

		// Returns a random postion between the defined outer and inner circle centered in position.
		Position generatePosition(Position position, float innerRadius, float outerRadius) {
			return position + (innerRadius + (outerRadius - innerRadius) * random()) * Position(std::cosf(random()*2.f*3.14f), std::sinf(random()*2.f*3.14f));
		}

	}

	ZombieGame::ZombieGame(const GameData& gameData) : engine_(this, gameData.getTimeStemMS(), gameData.getImpulseThreshold()), gameData_(gameData) {
		keyboard1_ = DevicePtr(new InputKeyboard(SDLK_UP, SDLK_DOWN, SDLK_LEFT,
			SDLK_RIGHT, SDLK_SPACE, SDLK_r, SDLK_LSHIFT, SDLK_e));

		scale_ = 1.f;

		addKeyListener([&](gui::Component& component, const SDL_Event& keyEvent) {
			keyboard1_->eventUpdate(keyEvent);
		});

		innerSpawnRadius_ = 0.f;
		outerSpawnRadius_ = 5.f;

		std::map<std::string, Unit2D*> units = gameData_.getUnits();
		// Add human.
		{
			State state(generatePosition(ORIGO, 0, 50), ORIGO, 0);
			Unit2D* human = new Unit2D(*units["Human"]);
			engine_.setHuman(keyboard1_, state, human);
			viewPosition_ = human->getPosition();
		}
		// Add zombies.
		{
			Unit2D* zombie = units["Zombie"];
			for (int i = 0; i < gameData.getUnitLevel(); ++i) {
				State state(generatePosition(ORIGO, 0, 50), ORIGO, 0);
				engine_.add(state, new Unit2D(*zombie));
			}
		}
		// Add cars.
		{
			const std::map<std::string, Car2D*>& cars = gameData_.getCars();
			for (int i = 0; i < 10; ++i) {
				for (auto& pair : cars) {
					Car2D* car = pair.second;
					State state(generatePosition(ORIGO, 0, 50), ORIGO, 0);
					// Engine takes the ownership.
					engine_.add(state, new Car2D(*car));
				}
			}
		}
		// Add buildings.
		{
			const auto& buildings = gameData_.getBuildings();
			for (Building2D* building : buildings) {
				// Engine takes the ownership.
				engine_.add(new Building2D(*building));
			}
		}

		terrain2D_ = gameData.getTerrain2D();

		setBackgroundColor(mw::Color(0, 0.3, 0));
	}

	ZombieGame::~ZombieGame() {
	}

	// Starts the game.
	void ZombieGame::startGame() {
		engine_.start();
	}

	void ZombieGame::draw(Uint32 deltaTime) {
		gui::Component::draw(deltaTime);

		// Draw map centered around first human player.
		glPushMatrix();
		gui::Dimension dim = getSize();
		glTranslated(dim.width_*0.5f, dim.height_*0.5f, 0);
		glScale2f(50);
		glScale2f(scale_);
		glTranslate2f(-viewPosition_);

		// Game is started?
		if (engine_.isStarted()) {
			terrain2D_.draw(deltaTime / 1000.f);
			engine_.update(deltaTime / 1000.f);
		} else {
			terrain2D_.draw(0);
			engine_.update(0);
		}

		glPopMatrix();
	}

	void ZombieGame::humanPosition(float x, float y) {
		viewPosition_ += 0.1f * (Position(x, y) - viewPosition_);
	}

	void ZombieGame::zoom(float scale) {
		scale_ *= scale;
	}

} // Namespace zombie.
