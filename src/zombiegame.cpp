#include "zombiegame.h"
#include "auxiliary.h"
#include "inputkeyboard.h"
#include "unit2d.h"
#include "car2d.h"
#include "weapon2d.h"
#include "weaponitem2d.h"
#include "building2d.h"
#include "terrain2d.h"
#include "blood.h"
#include "shot.h"

// External.
#include <mw/exception.h>

// Stl.
#include <cmath>
#include <sstream>

namespace zombie {

	namespace {

		// Returns a random postion between the defined outer and inner circle centered in position.
		Position generatePosition(Position position, float innerRadius, float outerRadius) {
			return position + (innerRadius + (outerRadius - innerRadius) * random()) * Position(std::cos(random()*2.f*PI), std::sin(random()*2.f*PI));
		}

		Position generatePosition(std::vector<Position> positions) {
			return positions[randomInt(0, positions.size() - 1)];
		}

		void removeDeadGraphicObjects(std::list<std::shared_ptr<Graphic>>& list) {
			list.remove_if([](const std::shared_ptr<Graphic>& ob) {
				return ob->toBeRemoved();
			});
		}

		void drawGraphicList(std::list<std::shared_ptr<Graphic>>& list, float deltaTime) {
			for (auto ob : list) {
				ob->draw(deltaTime);
			}
		}

	}

	ZombieGame::ZombieGame(const GameData& gameData) : engine_(*this, gameData.getTimeStepMS(), gameData.getImpulseThreshold()), gameData_(gameData) {
		keyboard_ = DevicePtr(new InputKeyboard(SDLK_UP, SDLK_DOWN, SDLK_LEFT,
			SDLK_RIGHT, SDLK_SPACE, SDLK_r, SDLK_LSHIFT, SDLK_e));

		scale_ = 2.f;
		lastSpawnTime_ = engine_.getTime();
		spawnPeriod_ = 0.5f;

		addKeyListener([&](gui::Component& component, const SDL_Event& keyEvent) {
			keyboard_->eventUpdate(keyEvent);
		});

		gameData.load(*this);

		innerSpawnRadius_ = gameData.getInnerSpawnRadius();
		outerSpawnRadius_ = gameData.getOuterSpawnRadius();
		spawningPoints_ = gameData.loadSpawningPoints();

		// Add human to engine.
		{
			State state(generatePosition(spawningPoints_), ORIGO, 0);
			Unit* human = new Unit2D(*human_);
			engine_.setHuman(keyboard_, state, human);
			viewPosition_ = human->getPosition();
			refViewPosition_ = viewPosition_;
		}

		// Add zombies to engine.
		for (int i = 0; i < gameData.getUnitLevel(); ++i) {
			Unit* zombie = new Unit2D(*zombie_);
			State state(generatePosition(spawningPoints_), ORIGO, 0);
			engine_.add(state, zombie);
		}

		// Add cars to engine.
		for (int i = 0; i < 15; ++i) {
			State state(generatePosition(spawningPoints_), ORIGO, 0);
			engine_.add(state, new Car2D(*car_));
		}

		setBackgroundColor(0, 0.1f, 0);
	}

	ZombieGame::~ZombieGame() {
	}

	void ZombieGame::updateEachCycle(Unit& unit, Unit& human) {
		Position diff = unit.getPosition() - human.getPosition();
		if (diff.LengthSquared() > outerSpawnRadius_*outerSpawnRadius_) {
			// Move to new postion and direction.
			float angle = 2 * PI * random();
			unit.getBody()->SetTransform(generatePosition(human.getPosition(), innerSpawnRadius_, outerSpawnRadius_), angle);
		}
	}

	// Starts the game.
	void ZombieGame::startGame() {
		engine_.start();
	}

	void ZombieGame::draw(Uint32 deltaTime) {
		gui::Component::draw(deltaTime);
		
		viewPosition_ += 10 * deltaTime/1000.f * (refViewPosition_ - viewPosition_);

		// Draw map centered around first human player.
		glPushMatrix();
		gui::Dimension dim = getSize();
		glTranslated(dim.width_*0.5f, dim.height_*0.5f, 0);
		glScale2f(50);
		glScale2f(scale_);
		glTranslate2f(-viewPosition_);

		// Game is started?
		if (engine_.isStarted()) {
			terrain_.draw(deltaTime / 1000.f);
			drawGraphicList(graphicGround_, deltaTime / 1000.f);
			engine_.update(deltaTime / 1000.f);
			drawGraphicList(graphicMiddle_, deltaTime / 1000.f);
			drawGraphicList(graphicHeaven_, deltaTime / 1000.f);

			removeDeadGraphicObjects(graphicGround_);
			removeDeadGraphicObjects(graphicMiddle_);
			removeDeadGraphicObjects(graphicHeaven_);
		} else {
			terrain_.draw(0);
			engine_.update(0);
		}

		glPopMatrix();
		refViewPosition_ = engine_.getHumanState().position_;
	}

	void ZombieGame::updateEachCycle(Unit& human) {
		if (engine_.getTime() - lastSpawnTime_ > spawnPeriod_) {
			lastSpawnTime_ = engine_.getTime();

			// Reduce spawnPeriod gradually
			float angle = 2 * PI * random();
			State state(generatePosition(human.getPosition(), innerSpawnRadius_, outerSpawnRadius_), ORIGO, angle);
			engine_.add(state, new Unit2D(*zombie_));
		}
	}

	void ZombieGame::zoom(float scale) {
		scale_ *= scale;
	}

	void ZombieGame::unitDied(Unit& unit) {
		graphicGround_.push_back(std::make_shared<Blood>(unit.getPosition()));
	}

	void ZombieGame::humanDied(Unit& unit) {
		graphicGround_.push_back(std::make_shared<Blood>(unit.getPosition()));
	}

	void ZombieGame::collision(float impulse, Car& car, Unit& unit) {
	}

	void ZombieGame::collision(float impulse, Car& car1, Car& car2) {
	}

	void ZombieGame::collision(float impulse, Car& car, Building& building) {
	}

	void ZombieGame::shotMissed(const Bullet& bullet, Position end) {
		graphicMiddle_.push_back(std::make_shared<Shot>(bullet, end));
	}

	void ZombieGame::shotHit(const Bullet& bullet, Unit& unit) {
		graphicMiddle_.push_back(std::make_shared<Shot>(bullet, unit.getPosition()));
		graphicGround_.push_back(std::make_shared<Blood>(unit.getPosition()));
	}

	// Implements the data interface.
	void ZombieGame::loadBuilding(const std::vector<Position>& corners) {
		engine_.add(new Building2D(corners));
	}

	void ZombieGame::loadZombie(float mass, float radius, float life, float walkingSpeed, float runningSpeed, float stamina, const Animation& animation, std::string weapon) {
		for (int i = 0; i < gameData_.getUnitLevel(); ++i) {
			zombie_ = std::unique_ptr<Unit2D>(new Unit2D(mass, radius, life, walkingSpeed, runningSpeed, true, weapons_[weapon].clone(), animation));
		}
	}

	void ZombieGame::loadHuman(float mass, float radius, float life, float walkingSpeed, float runningSpeed, float stamina, const Animation& animation, std::string weapon) {
		human_ = std::unique_ptr<Unit2D>(new Unit2D(mass, radius, life, walkingSpeed, runningSpeed, false, weapons_[weapon].clone(), animation));
	}

	void ZombieGame::loadCar(float mass, float width, float length, float life, const Animation& animation) {
		car_ = std::unique_ptr<Car2D>(new Car2D(mass, life, width, length, animation));
	}

	void ZombieGame::loadRoad(const std::vector<Position>& road) {
		terrain_.addRoad(road);
	}

	void ZombieGame::loadRoadLine(const std::vector<Position>& roadLine) {
		terrain_.addRoadLine(roadLine);
	}

	void ZombieGame::loadWater(const std::vector<Position>& corners) {
		terrain_.addWater(corners);
	}

	void ZombieGame::loadWeapon(std::string name, float damage, float timeBetweenShots, float range, int clipSize, const mw::Sprite& symbol, const Animation& animation) {
		weapons_[name] = Weapon2D(damage, timeBetweenShots, range, clipSize, symbol, animation);
	}

} // Namespace zombie.
