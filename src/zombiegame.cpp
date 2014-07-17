#include "zombiegame.h"
#include "inputkeyboard.h"
#include "weapon.h"
#include "movingobject.h"
#include "building.h"
#include "auxiliary.h"
#include "settings.h"
#include "unit2d.h"
#include "car2d.h"
#include "weaponitem2d.h"
#include "building2d.h"
#include "terrain2d.h"
#include "blood.h"

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
			return positions[randomInt(0, positions.size()-1)];
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

	ZombieGame::ZombieGame(const GameData& gameData) : engine_(this, gameData.getTimeStemMS(), gameData.getImpulseThreshold()), gameData_(gameData) {
		keyboard1_ = DevicePtr(new InputKeyboard(SDLK_UP, SDLK_DOWN, SDLK_LEFT,
			SDLK_RIGHT, SDLK_SPACE, SDLK_r, SDLK_LSHIFT, SDLK_e));

		scale_ = 1.f;
		lastSpawnTime_ = engine_.getTime();
		spawnPeriod_ = 0.2;

		addKeyListener([&](gui::Component& component, const SDL_Event& keyEvent) {
			keyboard1_->eventUpdate(keyEvent);
		});

		innerSpawnRadius_ = 25.f;
		outerSpawnRadius_ = 30.f;

		{
			std::map<std::string, Unit2D*> units = gameData_.getUnits();
			// Add human.
			{
				State state(generatePosition(gameData.getSpawningPoints()), ORIGO, 0);
				//State state(ORIGO, ORIGO, 0);
				Unit2D* human = new Unit2D(*units["Human"]);
				engine_.setHuman(keyboard1_, state, human);
				viewPosition_ = human->getPosition();
			}
			// Add zombies.
			{
				Unit2D* zombie = units["Zombie"];
				for (int i = 0; i < 0; ++i) { //gameData.getUnitLevel();
					State state(generatePosition(gameData.getSpawningPoints()), ORIGO, 0);
					engine_.add(state, new Unit2D(*zombie));
				}
			}
		}

		// Add cars.
		{
			const std::map<std::string, Car2D*>& cars = gameData_.getCars();
			for (int i = 0; i < 1; ++i) {
				for (auto& pair : cars) {
					Car2D* car = pair.second;
					State state(generatePosition(gameData.getSpawningPoints()), ORIGO, 0);
					engine_.add(state, new Car2D(*car));
				}
			}
		}
		// Add buildings.
		{
			const auto& buildings = gameData_.getBuildings();
			for (Building2D* building : buildings) {
				engine_.add(new Building2D(*building));
			}
		}

		terrain2D_ = gameData.getTerrain2D();

		setBackgroundColor(mw::Color(0, 0.3, 0));
	}

	ZombieGame::~ZombieGame() {
	}

	void ZombieGame::updateUnit(Unit& unit, Unit& human) {
		Position diff = unit.getPosition() - human.getPosition();
		if (diff.LengthSquared() > outerSpawnRadius_*outerSpawnRadius_) {
			double alfa = random() * 2 * PI;
			double dist = random() * (outerSpawnRadius_ - innerSpawnRadius_) + innerSpawnRadius_;
			Position p = dist * Position(std::cos(alfa), std::sin(alfa)) + human.getPosition();
			
			auto body = unit.getBody();
			
			// Move to new postion and direction.
			//body->SetTransform(p - body->GetPosition(), alfa - body->GetAngle());
			body->SetTransform(p, alfa);
		}
	}

	void ZombieGame::updateSpawning(Unit& human) {
		if (engine_.getTime() - lastSpawnTime_ > spawnPeriod_){
			lastSpawnTime_ = engine_.getTime();
			// Reduce spawnPeriod gradually

			double alfa = random() * 2 * PI;
			double dist = random() * (outerSpawnRadius_ - innerSpawnRadius_) + innerSpawnRadius_;
			Position p = dist * Position(std::cos(alfa), std::sin(alfa)) + human.getPosition();
			std::map<std::string, Unit2D*> units = gameData_.getUnits();
			Unit2D* zombie = units["Zombie"];
			State state(p, ORIGO, 0);
			engine_.add(state, new Unit2D(*zombie));
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
			terrain2D_.draw(deltaTime / 1000.f);
			drawGraphicList(graphicGround_, deltaTime / 1000.f);
			engine_.update(deltaTime / 1000.f);
			drawGraphicList(graphicMiddle_, deltaTime / 1000.f);
			drawGraphicList(graphicHeaven_, deltaTime / 1000.f);
			
			removeDeadGraphicObjects(graphicGround_);
			removeDeadGraphicObjects(graphicMiddle_);
			removeDeadGraphicObjects(graphicHeaven_);
		} else {
			terrain2D_.draw(0);
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

} // Namespace zombie.
