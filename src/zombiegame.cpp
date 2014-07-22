#include "zombiegame.h"
#include "inputkeyboard.h"
#include "building.h"
#include "auxiliary.h"
#include "settings.h"
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
			return position + (innerRadius + (outerRadius - innerRadius) * random()) * Position(std::cos(random()*2.f*3.14f), std::sin(random()*2.f*3.14f));
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

	ZombieGame::ZombieGame(const GameData& gameData) : engine_(*this, gameData.getTimeStemMS(), gameData.getImpulseThreshold()), gameData_(gameData) {
		keyboard1_ = DevicePtr(new InputKeyboard(SDLK_UP, SDLK_DOWN, SDLK_LEFT,
			SDLK_RIGHT, SDLK_SPACE, SDLK_r, SDLK_LSHIFT, SDLK_e));

		scale_ = 1.f;
		lastSpawnTime_ = engine_.getTime();
		spawnPeriod_ = 0.2f;

		addKeyListener([&](gui::Component& component, const SDL_Event& keyEvent) {
			keyboard1_->eventUpdate(keyEvent);
		});

		innerSpawnRadius_ = 25.f;
		outerSpawnRadius_ = 30.f;
		
		spawningPoints_ = gameData.loadSpawningPoints();

		gameData.load(*this);

		// Add human to engine.
		{
			State state(generatePosition(spawningPoints_), ORIGO, 0);
			Unit* human = new Unit2D(*human_);
			engine_.setHuman(keyboard1_, state, human);
			viewPosition_ = human->getPosition();
		}

		// Add zombies to engine.
		for (int i = 0; i < 40; ++i) { //gameData.getUnitLevel();
			Unit* zombie = new Unit2D(*zombie_);
			State state(generatePosition(spawningPoints_), ORIGO, 0);
			engine_.add(state, zombie);
		}

		// Add cars to engine.
		for (int i = 0; i < 15; ++i) {
			State state(generatePosition(spawningPoints_), ORIGO, 0);
			engine_.add(state, new Car2D(*car_));
		}

		setBackgroundColor(mw::Color(0, 0.4, 0));
	}

	ZombieGame::~ZombieGame() {
	}

	void ZombieGame::updateUnit(Unit& unit, Unit& human) {
		Position diff = unit.getPosition() - human.getPosition();
		if (diff.LengthSquared() > outerSpawnRadius_*outerSpawnRadius_) {
			float alfa = random() * 2 * PI;
			float dist = random() * (outerSpawnRadius_ - innerSpawnRadius_) + innerSpawnRadius_;
			Position p = dist * Position(std::cos(alfa), std::sin(alfa)) + human.getPosition();

			auto body = unit.getBody();

			// Move to new postion and direction.
			body->SetTransform(p, alfa);
		}
	}

	void ZombieGame::updateSpawning(Unit& human) {
		if (engine_.getTime() - lastSpawnTime_ > spawnPeriod_) {
			lastSpawnTime_ = engine_.getTime();
			// Reduce spawnPeriod gradually

			float alfa = random() * 2 * PI;
			float dist = random() * (outerSpawnRadius_ - innerSpawnRadius_) + innerSpawnRadius_;
			Position p = dist * Position(std::cos(alfa), std::sin(alfa)) + human.getPosition();
			
			State state(p, ORIGO, 0);
			engine_.add(state, new Unit2D(*zombie_));			
		}
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
	}

	void ZombieGame::currentHuman(Unit& unit) {
		viewPosition_ += 0.1f * (unit.getPosition() - viewPosition_);
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

	void ZombieGame::shotMissed(const Bullet& bullet) {
		Position endPosition = bullet.postion_ + bullet.range_ * Position(std::cos(bullet.direction_), std::sin(bullet.direction_));
		graphicMiddle_.push_back(std::make_shared<Shot>(bullet, endPosition));
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
		for (int i = 0; i < 40; ++i) { //gameData.getUnitLevel();
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
