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

#include "zombiebehavior.h"
#include "survivorbehavior.h"

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
#include <mw/quadtree.h>

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
		world_->SetContactListener(this);

		// Set windows size.
		updateSize(width,height);

		taskManager_ = new TaskManager();		
		scale_ = 1.0;

		started_ = false;
		time_ = 0.0f;
				
		unitLevel_ = 200;
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
			spawnAndCleanUpUnits();
			
			// Update the view.
			for (PairPlayerUnit& pair : players_) {
				Unit* unit = dynamic_cast<Unit*>(pair.second);
				if (unit) {
					std::vector<Unit*> unitsInView = calculateUnitsInView(unit);
					pair.first->updateUnitsInView(unitsInView);
				}
			}

			// Calculate all inputs.
			for (auto& pair : players_) {
				PlayerPtr player = pair.first;
				Unit* unit = dynamic_cast<Unit*>(pair.second);
				if (unit) {
					player->calculateInput(unit,time_);
				}
			}

			// Update all units.
			for (PairPlayerUnit& pair : players_) {
				MovingObject* movingOb = std::get<1>(pair);
				Input input = std::get<0>(pair)->currentInput();
				movingOb->updatePhysics(time_, timeStep,input);
			}

			// Update the objects physics interactions.
			world_->Step(timeStep,6,2);

			// Move the time ahead.
			time_ += timeStep;
		}
	}

	void ZombieGame::spawnAndCleanUpUnits() {
		Position center = players_.front().second->getBody()->GetPosition();

		// Delete units outside of perimiter.
		MovingObject* temp = nullptr;
		players_.remove_if([&](const PairPlayerUnit& pair) {
			Position unitPos = pair.second->getBody()->GetPosition();
			if ( (unitPos-center).LengthSquared() > outerSpawnRadius_*outerSpawnRadius_ ) {
				pair.second->kill();
				return true;
			}
			return false;
		});

		// Spawn new units.
		int nbrOfZombies = players_.size();
		while (nbrOfZombies < unitLevel_) {
			// Insert zombie.
			Position p = map_.generateSpawnPosition(center,innerSpawnRadius_,outerSpawnRadius_);
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
		glPushMatrix();

		b2Body* body = players_.front().second->getBody();

		if (body != nullptr) {
			Position p = body->GetPosition();
			viewPosition_ += deltaTime * (p - viewPosition_);
		}

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
			sdlEventSignal_(windowEvent);
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
		players_.push_back(PairPlayerUnit(human,unit));
	}

	void ZombieGame::addNewAi(Unit* unit) {
		AiBehaviorPtr b;
		if (unit->isInfected()) {
			taskManager_->add(new ZombieAnimation(unit));
			b = std::make_shared<ZombieBehavior>();
		} else {
			taskManager_->add(new HumanAnimation(unit));
			b = std::make_shared<SurvivorBehavior>();
		}
		AiPlayerPtr aiPlayer(new AiPlayer(b));
		players_.push_back(PairPlayerUnit(aiPlayer,unit));
	}

	void ZombieGame::addNewCar(Car* car) {
		taskManager_->add(new CarAnimation(car));
	}

	void ZombieGame::initGame() {
		taskManager_->add(new SurvivalTimer());		

		//map_ = loadMapInfo("housesFME.mif","roadsFME.mif", 1);
		map_ = loadMapInfo("housesFME.mif","roadsFME.mif", 1);
		//Map mapTile1 = loadTile("housesFME.mif","roadsFME.mif",100);
		//map_ = createTiledMap(mapTile1);

		taskManager_->add(new MapDraw(map_));
		taskManager_->add(new RoadDraw(map_));

		auto buildings = map_.getBuildings();

		for (BuildingPtr building : buildings) {
			BuildingPtr tmp(new Building(world_,building->getCorners()));
			taskManager_->add(new DrawFake3DBuildning(tmp));
		}

		// Add human controlled by first input device.
		Position position = map_.generateSpawnPosition();
		Unit* human = createUnit(position.x,position.y,0.3f,Weapon(55,0.2f,8,12),false);
		viewPosition_ = human->getPosition();

		HumanPlayerPtr humanPlayer(new InputKeyboard(SDLK_UP,SDLK_DOWN,SDLK_LEFT,SDLK_RIGHT,SDLK_SPACE,SDLK_r,SDLK_LSHIFT));
		addHuman(humanPlayer,human);
		auto connection = sdlEventSignal_.connect(std::bind(&HumanPlayer::eventUpdate,humanPlayer,std::placeholders::_1));
		humanPlayer->setConnection(connection);

		{
			humanPlayer = HumanPlayerPtr(new InputKeyboard(SDLK_w,SDLK_s,SDLK_a,SDLK_d,SDLK_f,SDLK_g,SDLK_h));
			Position spawn = map_.generateSpawnPosition(human->getPosition(),innerSpawnRadius_,outerSpawnRadius_);
			Car* car = new Car(spawn.x,spawn.y);
			players_.push_back(PairPlayerUnit(humanPlayer,car));
			addNewCar(car);
			auto connection = sdlEventSignal_.connect(std::bind(&HumanPlayer::eventUpdate,humanPlayer,std::placeholders::_1));
			humanPlayer->setConnection(connection);
		}

		// Add zombie with standard behavior.
		for (int i = 30; i < 40; i++) {
			Position spawn = map_.generateSpawnPosition(human->getPosition(),innerSpawnRadius_,outerSpawnRadius_);
			Unit* zombie = createUnit(spawn.x,spawn.y,0.3f,Weapon(35,0.5f,1,12),true);
			addNewAi(zombie);
		}

		for (int i = 1; i < 100; i++) {
			Position spawn = map_.generateSpawnPosition(human->getPosition(),1,10);
			Unit* survivor = createUnit(spawn.x,spawn.y,0.f,Weapon(35,0.5,8,120),false);
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

	void ZombieGame::doAction(Unit* unit) {
		float angle = unit->getState().angle_;
		b2Vec2 dir(std::cos(angle),std::sin(angle));
		ClosestRayCastCallback callback;
		world_->RayCast(&callback,unit->getPosition(),unit->getPosition() + dir);

		b2Fixture* fixture = callback.getClosest();

		// Is there an object near by?
		if (fixture != nullptr) {
			Object* ob = static_cast<Object*>(fixture->GetUserData());

			if (Car* car = dynamic_cast<Car*>(ob)) {
				// Driver seat is empty.
				if (car->getDriver() != nullptr) {
					car->setDriver(unit);
				}
			}
		}
	}

	void ZombieGame::doShotDamage(Unit* shooter, const Bullet& bullet) {
		b2Vec2 dir(std::cos(bullet.direction_),std::sin(bullet.direction_));
		b2Vec2 endP = shooter->getPosition() + bullet.range_ * dir;

		ClosestRayCastCallback callback;		

		world_->RayCast(&callback,shooter->getPosition(),endP);
		b2Fixture* fixture = callback.getClosest();

		// Did bullet hit something?
		if (fixture != nullptr) {
			Object* ob = static_cast<Object*>(fixture->GetUserData());

			if (Unit* target = dynamic_cast<Unit*>(ob)) {
				// Target alive?
				if (!target->isDead()) {
					target->updateHealthPoint(-bullet.damage_);
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

	bool ZombieGame::isVisible(Unit* unitToBeSeen, Unit* unitThatSees) const {
		return true;
	}

	void ZombieGame::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) {
		if (std::abs(impulse->normalImpulses[0]) > 0.8f) {
			Object* ob1 = static_cast<Object*>(contact->GetFixtureA()->GetUserData());
			if (Unit* unit = dynamic_cast<Unit*>(ob1)) {
				unit->updateHealthPoint(-101.0);
				Position p = unit->getPosition();
				if (unit->isDead()) {
					taskManager_->add(new Blood(p.x,p.y,time_));
					taskManager_->add(new BloodStain(p.x,p.y,time_));
				} else {
					taskManager_->add(new BloodSplash(p.x,p.y,time_));
				}
			}

			Object* ob2 = static_cast<Object*>(contact->GetFixtureB()->GetUserData());
			if (Unit* unit = dynamic_cast<Unit*>(ob2)) {
				unit->updateHealthPoint(-101.0);
				Position p = unit->getPosition();
				if (unit->isDead()) {
					taskManager_->add(new Blood(p.x,p.y,time_));
					taskManager_->add(new BloodStain(p.x,p.y,time_));
				} else {
					taskManager_->add(new BloodSplash(p.x,p.y,time_));
				}

			}
		}
	}
	
} // Namespace zombie.
