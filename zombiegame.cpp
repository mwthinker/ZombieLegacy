#include "zombiegame.h"

#include "typedefs.h"
#include "inputkeyboard.h"
#include "unit.h"
#include "building.h"
#include "bullet.h"
#include "map.h"
#include "tile.h"
#include "input.h"
#include "task.h"
#include "quadtree.h"
#include "taskmanager.h"
#include "aiplayer.h"
#include "survivaltimer.h"
#include "graphictask.h"
#include "car.h"
#include "blood.h"
#include "caranimation.h"

#include <Box2D/Box2D.h>
#include <SDL.h>
#include <memory>
#include <vector>

#include <string>
#include <fstream>
#include <gl/GLU.h>
#include <tuple>

namespace zombie {

	namespace {

		class InViewQueryCallback : public b2QueryCallback {
		public:
			InViewQueryCallback() {

			}

			std::vector<b2Fixture*> foundFixtures;

			bool ReportFixture(b2Fixture* fixture) {
				foundFixtures.push_back(fixture);
				return true;//keep going to find all fixtures in the query area
			}

			void reset() {
				foundFixtures.clear();
			}
		};

		class ClosestRayCastCallback : public b2RayCastCallback {
		public:
			ClosestRayCastCallback() {
				closestFraction_ = 100.f;
				closest_ = nullptr;
			}

			b2Fixture* getClosest() const {
				return closest_;
			}

			// Ray-cast callback.
			float32 ReportFixture(b2Fixture* fixture, const b2Vec2 &point, const b2Vec2 &normal, float32 fraction) override {
				closest_ = fixture;
				return fraction;//keep going to find all fixtures in the query area
			}

			void reset() {
				closestFraction_ = 100.f;
				closest_ = nullptr;
			}
		private:
			b2Fixture* closest_;
			float closestFraction_;
		};

	}

	ZombieGame::ZombieGame(int width, int height) {
		// Create a world with no "gravity".
		world_ = new b2World(b2Vec2(0,0));
		Object::setWorld(world_);

		// Set windows size.
		updateSize(width,height);

		taskManager_ = new TaskManager();		
		scale_ = 1.0;

		started_ = false;
		time_ = 0.0f;
		timeToUpdateView_ = 0.25f; // Seconds in which the view is assumed to be constant in order to 
		
		// Speed up calculations.
		timeToUpdateSpawn_ = 0.5f; // Time between spawns and unit clean ups.
		timeSinceSpawn_ = 0.0f;
		indexAiPlayer_ = 0;
		unitLevel_ = 50;
		innerSpawnRadius_ = 10.f;
		outerSpawnRadius_ = 40.f;

		timeStep_ = 0.017f; // Fix time step for physics update.
		accumulator_ = 0.0f; // Time accumulator.

		initGame();
	}

	ZombieGame::~ZombieGame() {
		delete taskManager_;
		delete world_;
	}

	void ZombieGame::startGame() {
		// Start game!
		started_ = true;
	}

	void ZombieGame::updatePhysics(float timeStep) {
		// Game is started?
		if (started_) {
			// Spawn and clean up units
			if (timeSinceSpawn_ > timeToUpdateSpawn_) {
				spawnAndCleanUpUnits();
				timeSinceSpawn_ = 0;
			}
			timeSinceSpawn_ += timeStep;

			// (deltaTime) must always be larger than (timeToUpdateView_).
			int nbrOfUnitsToUpdateViewOn = (int) (aiPlayers_.size() * (timeStep/timeToUpdateView_));
			// Update some of the ai:s view.
			for (int i = 0; i < nbrOfUnitsToUpdateViewOn; ++i) {
				indexAiPlayer_ = (indexAiPlayer_ + 1) % aiPlayers_.size();
				AiPlayerPtr& aiPlayer = aiPlayers_[indexAiPlayer_].first;
				Unit* unit = aiPlayers_[indexAiPlayer_].second;
				std::vector<Unit*> unitsInView = calculateUnitsInView(unit);
				aiPlayer->updateUnitsInView(unitsInView);
			}

			// Calculate all local ai:s input.
			for (auto& pair : aiPlayers_) {
				AiPlayerPtr& aiPlayer = pair.first;
				Unit* unit = pair.second;
				aiPlayer->calculateInput(unit,time_);				
			}

			// Update all units.
			for (PairPlayerUnit& pair : players_) {
				Unit* unit = std::get<1>(pair);
				Car* car = std::get<2>(pair);
				Input input = std::get<0>(pair)->currentInput();
				unit->updatePhysics(time_, timeStep,input);
				b2Body* body = unit->getBody();
				body->ApplyForceToCenter(-body->GetLinearVelocity());
			}

			// Update the objects physics interactions.
			world_->Step((float)timeStep,6,2);

			// Move the time ahead.
			time_ += timeStep;
		}
	}

	void ZombieGame::spawnAndCleanUpUnits() {
		Position center = humanPlayers_[0].second->getPosition();

		// delete zombies outside of perimiter ***************
		Unit* temp = nullptr;
		for (auto it = players_.begin(); it != players_.end(); it++) {
			Position unitPos = std::get<1>(*it)->getPosition();
			if ((unitPos-center).LengthSquared() > outerSpawnRadius_*outerSpawnRadius_) {
				// REMOVE UNIT
				std::get<1>(*it)->setIsDead();
				temp = std::get<1>(*it);
				players_.erase(it);
				break;
			}
		}

		for (auto it = aiPlayers_.begin(); it != aiPlayers_.end(); it++) {
			if (it->second == temp) {
				aiPlayers_.erase(it);
				break;
			}
		}

		// spawn new units ************************************
		int nbrOfZombies = aiPlayers_.size();
		while (nbrOfZombies < unitLevel_) {
			// INSERT ZOMBIE
			Position p = map_.generateSpawnPosition(humanPlayers_[0].second->getPosition(),innerSpawnRadius_,outerSpawnRadius_);
			Unit* zombie = createUnit(p.x,p.y,0.3f,Weapon(25,0.5f,1,12),true);
			addNewAi(zombie);
			nbrOfZombies++;
		}
	}

	void ZombieGame::update(float deltaTime) {
		// DeltaTime to big?
		if (deltaTime > 0.250) {
			// To avoid spiral of death.
			deltaTime = 0.250;
		}

		accumulator_ += deltaTime;
		while (accumulator_ >= timeStep_) {
			accumulator_ -= timeStep_;		
			updatePhysics(timeStep_);
		}

		// Draw map centered around first humna player.
		Unit* unit = humanPlayers_[0].second;
		glPushMatrix();

		Position p = unit->getPosition();
		viewPosition_ += deltaTime * (unit->getPosition() - viewPosition_);

		glTranslated(0.5, 0.5, 0);
		glScaled(1.0/50,1.0/50,1); // Is to fit the box drawn where x=[0,1] and y=[0,1].

		drawCircle(0,0,0.5,20,false);
		glScaled(scale_,scale_,1); // Is to fit the box drawn where x=[0,1] and y=[0,1].
		drawCircle(0,0,0.5,20,false);				

		glTranslated(-viewPosition_.x,-viewPosition_.y,0.0);

		// Game is started?
		if (started_) {
			taskManager_->update(deltaTime);
		} else {
			taskManager_->update(0.0);
		}
		glPopMatrix();
	}

	void ZombieGame::eventUpdate(const SDL_Event& windowEvent) {
		// Game is started?
		if (started_) {
			// Update all human input.
			for (auto& pair : humanPlayers_) {
				HumanPlayerPtr& humanPlayer = pair.first;
				humanPlayer->eventUpdate(windowEvent);
			}
		}
	}

	Unit* ZombieGame::createUnit(float x, float y, float angle, const Weapon& weapon, bool infected) {
		Unit* unit = new Unit(x, y, angle, weapon, infected);
		unit->addShootHandler(std::bind(&ZombieGame::doShotDamage,this,std::placeholders::_1,std::placeholders::_2));
		return unit;
	}

	void ZombieGame::addHuman(HumanPlayerPtr human, Unit* unit) {
		taskManager_->add(new HumanAnimation(unit));
		taskManager_->add(new HumanStatus(unit,HumanStatus::ONE));
		humanPlayers_.push_back(PairHumanUnit(human,unit));
		players_.push_back(PairPlayerUnit(human,unit,nullptr));		
	}

	void ZombieGame::addNewAi(Unit* unit) {
		if (unit->isInfected()) {
			taskManager_->add(new ZombieAnimation(unit));
		} else {
			taskManager_->add(new HumanAnimation(unit));
		}
		AiPlayerPtr aiPlayer(new AiPlayer());
		aiPlayers_.push_back(PairAiUnit(aiPlayer,unit));
		players_.push_back(PairPlayerUnit(aiPlayer,unit,nullptr));
	}

	void ZombieGame::initGame() {
		taskManager_->add(new SurvivalTimer());		

		//map_ = loadMapInfo("housesFME.mif","roadsFME.mif", 1);
		map_ = loadMapInfo("housesFME.mif","roadsFME.mif", 1);
		//Map mapTile1 = loadTile("housesFME.mif","roadsFME.mif",100);
		//map_ = createTiledMap(mapTile1);

		buildings_ = Quadtree<BuildingPtr>(map_.minX(),map_.minY(),map_.width(),map_.height(),4);

		taskManager_->add(new MapDraw(map_));
		taskManager_->add(new RoadDraw(map_));

		auto buildings = map_.getBuildings();

		for (BuildingPtr building : buildings) {
			BuildingPtr tmp(new Building(world_,building->getCorners()));
			taskManager_->add(new DrawFake3DBuildning(tmp));
			//buildings_.add(tmp,tmp->getPosition().x_,tmp->getPosition().y_,tmp->getRadius());
		}

		// Add human controlled by first input device.
		Position position = map_.generateSpawnPosition();
		Unit* human = createUnit(position.x,position.y,0.3f,Weapon(55,0.2f,8,12),false);
		viewPosition_ = human->getPosition();

		HumanPlayerPtr humanPlayer(new InputKeyboard(SDLK_UP,SDLK_DOWN,SDLK_LEFT,SDLK_RIGHT,SDLK_SPACE,SDLK_r,SDLK_LSHIFT));
		addHuman(humanPlayer,human);
		//humanPlayer = HumanPlayerPtr(new InputKeyboard(SDLK_w,SDLK_s,SDLK_a,SDLK_d,SDLK_f,SDLK_g,SDLK_h));
		//addHuman(humanPlayer,human);

		// Add zombie with standard behavior.
		for (int i = 30; i < 40; i++) {
			Position spawn = map_.generateSpawnPosition(human->getPosition(),innerSpawnRadius_,outerSpawnRadius_);
			Unit* zombie = createUnit(spawn.x,spawn.y,0.3f,Weapon(35,0.5f,1,12),true);
			addNewAi(zombie);
		}

		for (int i = 5; i < 5; i++) {
			Position spawn = map_.generateSpawnPosition(human->getPosition(),1,innerSpawnRadius_);
			Unit* survivor = createUnit(spawn.x,spawn.x,0.f,Weapon(35,0.5,8,12),false);
			addNewAi(survivor);
		}
	}

	void ZombieGame::zoom(double scale) {
		scale_ *= scale;
	}

	void ZombieGame::updateSize(int width, int height) {
		Task::width = width;
		Task::height = height;
	}

	std::vector<Unit*> ZombieGame::calculateUnitsInView(Unit* unit) {
		b2AABB area;
		b2Vec2 dist(unit->viewDistance(),unit->viewDistance());
		area.upperBound = dist + unit->getPosition();
		area.lowerBound = -dist + unit->getPosition();

		InViewQueryCallback queryCallback;

		world_->QueryAABB(&queryCallback,area);

		// Find all units in view.
		std::vector<Unit*> unitsInView;
		for (unsigned int i = 0; i < queryCallback.foundFixtures.size(); i++) {
			Object* ob = static_cast<Object*>(queryCallback.foundFixtures[i]->GetUserData());

			if (Unit* unitInArea = dynamic_cast<Unit*>(ob)) {
				Position p = unitInArea->getPosition();
				if (unitInArea != unit && 
					(unit->isInsideSmalViewDistance(p.x,p.y)) || (unit->isPointViewable(p.x,p.y)) ) {
						unitsInView.push_back(unitInArea);
				}
			}
		}

		return unitsInView;
	}

	void ZombieGame::doShotDamage(Unit* shooter, const Bullet& bullet) {
		b2Vec2 dir(std::cos(bullet.direction_),std::sin(bullet.direction_));
		b2Vec2 endP = shooter->getPosition() + bullet.range_ * dir;
		lastBullet_ = bullet;

		ClosestRayCastCallback callback;		

		world_->RayCast(&callback,shooter->getPosition(),endP);
		b2Fixture* fixture = callback.getClosest();

		// Did bullet hit something?
		if (fixture != nullptr) {
			Object* ob = static_cast<Object*>(fixture->GetUserData());

			if (Unit* target = dynamic_cast<Unit*>(ob)) {
				// Target alive?
				if (!target->isDead()) {
					target->updateHealthPoint(-lastBullet_.damage_);
					endP = target->getPosition();
					// Target killed?
					if (target->isDead()) {
						taskManager_->add(new Blood(endP.x,endP.y,time_));
						taskManager_->add(new BloodStain(endP.x,endP.y,time_));
					} else {
						taskManager_->add(new BloodSplash(endP.x,endP.y,time_));
					}
				}
			}
		} else {
			std::cout << endP.x << " " << endP.y << std::endl;
		}

		taskManager_->add(new Shot(shooter->getPosition(),endP,time_));
		std::cout << endP.x << " " << endP.y << std::endl;

	}

	bool ZombieGame::isVisible(UnitPtr unitToBeSeen, UnitPtr unitThatSees) const {
		return true;
	}
} // namespace zombie