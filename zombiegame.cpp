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
#include "weaponobject.h"
#include "shot.h"
#include "mapdraw.h"
#include "roaddraw.h"
#include "drawweaponobject.h"
#include "buildingdraw.h"

#include "zombiebehavior.h"
#include "survivorbehavior.h"

#include "gamesound.h"
#include "taskmanager.h"
#include "aiplayer.h"
#include "survivaltimer.h"
#include "graphictask.h"
#include "car.h"
#include "blood.h"
#include "caranimation.h"
#include "zombieanimation.h"
#include "humananimation.h"
#include "humanplayer.h"

#include <Box2D/Box2D.h>
#include <SDL.h>
#include <SDL_opengl.h>

#include <memory>
#include <vector>
#include <string>

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
				// Is a physical fixture?
				if (!fixture->IsSensor()) {
					closest_ = fixture;
				}
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

		bool getVisibleObject(b2Contact* contact, MovingObject*& target, MovingObject*& looker) {
			b2Fixture* fixtureA = contact->GetFixtureA();
			b2Fixture* fixtureB = contact->GetFixtureB();

			// Make sure only one of the fixtures was a sensor.
			// Assumes that the first fixture in the fixture list is the tensor.
			bool sensorA = fixtureA->IsSensor();
			bool sensorB = fixtureB->IsSensor();
			if (sensorA == sensorB) {
				return false;
			}

			Object* ob1 = static_cast<Object*>(fixtureA->GetUserData());
			Object* ob2 = static_cast<Object*>(fixtureB->GetUserData());
			MovingObject* mOb1 = dynamic_cast<MovingObject*>(ob1);
			MovingObject* mOb2 = dynamic_cast<MovingObject*>(ob2);

			// Make sure both are moving objects.
			if (mOb1 && mOb2) {
				if (sensorA) {
					looker = mOb1;
					target = mOb2;
				} else {
					looker = mOb2;
					target = mOb1;
				}
				return true;
			}

			return false;
		}
	}

	ZombieGame::ZombieGame(int width, int height) {
		// Create a world with no "gravity".
		world_ = new b2World(b2Vec2(0,0));
		Object::world = world_;
		worldHash_[0] = nullptr;
		Object::worldHash = &worldHash_;

		world_->SetContactListener(this);

		// Set windows size.
		updateSize(width,height);

		taskManager_ = new TaskManager();
		keyboard1_ = DevicePtr(new InputKeyboard(SDLK_UP,SDLK_DOWN,SDLK_LEFT,SDLK_RIGHT,SDLK_SPACE,SDLK_r,SDLK_LSHIFT,SDLK_e));
		keyboard2_ = DevicePtr(new InputKeyboard(SDLK_w,SDLK_s,SDLK_a,SDLK_d,SDLK_f,SDLK_g,SDLK_h,SDLK_z));
		
		scale_ = 1.0;

		started_ = false;
		time_ = 0.0f;

		unitLevel_ = 20;
		innerSpawnRadius_ = 10.f;
		outerSpawnRadius_ = 20.f;

		timeStep_ = 0.017f; // Fix time step for physics update.
		accumulator_ = 0.0f; // Time accumulator.

		initGame();
	}

	ZombieGame::~ZombieGame() {
		// Remove all game objects.
		std::vector<Object*> objects;
		for (b2Body* b = world_->GetBodyList(); b; b = b->GetNext()) {
			Object* ob = static_cast<Object*>(b->GetUserData());
			objects.push_back(ob);
		}
		for (Object* ob : objects) {
			delete ob;
		}

		// When all game objects are removed then remove world.
		delete world_;

		// Remove all tasks depending on gameobjects.
		delete taskManager_;
	}

	void ZombieGame::startGame() {
		// Start game!
		started_ = true;
	}

	void ZombieGame::updatePhysics(float timeStep) {
		// Game is started?
		if (started_) {
			spawnAndCleanUpUnits();
			
			// Update all players.
			for (Player* player: players_) {
				Object* ob = worldHash_[player->getId()];
				if (ob != nullptr) {
					MovingObject* mOb = static_cast<MovingObject*>(ob);
					player->calculateInput(time_);
					mOb->updatePhysics(time_, timeStep, player->currentInput());
				}
			}

			// Update the objects physics interactions.
			world_->Step(timeStep, 4, 2);

			// Move the time ahead.
			time_ += timeStep;
		}
	}

	void ZombieGame::spawnAndCleanUpUnits() {
		const MovingObject* frontOb = static_cast<const MovingObject*>(worldHash_[players_.front()->getId()]);
		Position center = frontOb->getPosition();
		
		// Delete units outside of perimiter and dead units.
		MovingObject* temp = nullptr;
		players_.remove_if([&](Player* player) {
			Object*& ob = worldHash_[player->getId()];
			if (ob != nullptr) {
				const MovingObject* mOb = static_cast<const MovingObject*>(ob);
				
				bool outside = (mOb->getPosition() - center).LengthSquared() > outerSpawnRadius_ * outerSpawnRadius_;
				bool dead = mOb->isDead();

				if (outside || dead) {
					delete player;
					delete ob;
					ob = nullptr;
					return true;
				}
			}
			
			return false;
		});

		// Spawn new units.
		int nbrOfZombies = players_.size();
		while (nbrOfZombies < unitLevel_) {
			// Insert zombie.
			Position p = map_.generateSpawnPosition(center, innerSpawnRadius_, outerSpawnRadius_);
			Unit* zombie = createUnit(p.x,p.y,0.3f,Weapon(25,0.5f,1,10000),true);
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

		const MovingObject* frontOb = static_cast<const MovingObject*>(worldHash_[players_.front()->getId()]);

		if (frontOb != nullptr) {
			viewPosition_ += deltaTime * (frontOb->getPosition() - viewPosition_);
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
			keyboard1_->eventUpdate(windowEvent);
			keyboard2_->eventUpdate(windowEvent);
		}
	}

	Unit* ZombieGame::createUnit(float x, float y, float angle, const Weapon& weapon, bool infected) {
		Unit* unit = new Unit(x, y, angle, weapon, infected);
		unit->addShootHandler(std::bind(&ZombieGame::doShotDamage,this,std::placeholders::_1,std::placeholders::_2));
		unit->addActionHandler(std::bind(&ZombieGame::doAction,this,std::placeholders::_1));
		worldHash_[unit->getId()] = unit;
		return unit;
	}

	void ZombieGame::addHuman(DevicePtr device, Unit* unit) {
		players_.push_back(new HumanPlayer(device, unit));
		taskManager_->add(new HumanStatus(unit, HumanStatus::ONE));
		taskManager_->add(new HumanAnimation(unit));
	}

	void ZombieGame::addNewAi(Unit* unit) {
		if (unit->isInfected()) {
			AiBehaviorPtr b = std::make_shared<ZombieBehavior>();
			taskManager_->add(new ZombieAnimation(unit));
			AiPlayer* aiPlayer(new AiPlayer(b, unit));
			players_.push_back(aiPlayer);
		} else {
			AiBehaviorPtr b = std::make_shared<SurvivorBehavior>();
			taskManager_->add(new HumanAnimation(unit));
			AiPlayer* aiPlayer(new AiPlayer(b, unit));
			players_.push_back(aiPlayer);
		}
	}

	void ZombieGame::initGame() {
		taskManager_->add(new SurvivalTimer());

		map_ = loadMapInfo("housesFME.mif","roadsFME.mif", 1);
		taskManager_->add(new MapDraw(map_));
		taskManager_->add(new RoadDraw(map_));

		auto buildings = map_.getBuildings();
		for (Building* building : buildings) {
			taskManager_->add(new BuildingDraw(building));
		}

		// Add human controlled by first input device.
		Position position = map_.generateSpawnPosition();
		Unit* human = createUnit(position.x,position.y,0.3f,Weapon(55,0.2f,8,12),false);
		viewPosition_ = human->getPosition();
		addHuman(keyboard1_, human);
		{
			Position spawn = map_.generateSpawnPosition(human->getPosition(),innerSpawnRadius_,outerSpawnRadius_);
			Car* car = new Car(spawn.x,spawn.y);
			players_.push_back(new HumanPlayer(keyboard2_,car));
			worldHash_[car->getId()] = car;
			taskManager_->add(new CarAnimation(car));
		}

		// Add zombie with standard behavior.
		for (int i = 0; i < 1; i++) {
			Position spawn = map_.generateSpawnPosition(human->getPosition(), innerSpawnRadius_, outerSpawnRadius_);
			Unit* zombie = createUnit(spawn.x, spawn.y, 0.3f, Weapon(35,0.5f,1,10000), true);
			addNewAi(zombie);
		}

		for (int i = 1; i < 10; i++) {
			Position spawn = map_.generateSpawnPosition(human->getPosition(),1,10);
			Unit* survivor = createUnit(spawn.x, spawn.y, 0.f, Weapon(35,0.5,8,120), false);
			addNewAi(survivor);
		}

		for (int i = 1; i < 15; i++) {
			Position spawn = map_.generateSpawnPosition(human->getPosition(),1,50);
			Weapon weapon;
			taskManager_->add(new DrawWeaponObject(new WeaponObject(spawn.x, spawn.y, weapon)));
		}
	}

	void ZombieGame::zoom(double scale) {
		scale_ *= scale;
	}

	void ZombieGame::updateSize(int width, int height) {
		Task::width = width;
		Task::height = height;
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
			} else if (WeaponObject* wOb = dynamic_cast<WeaponObject*>(ob)) {
				unit->setWeapon(wOb->getWeapon());
				delete wOb;
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
			//std::cout << endP.x << " " << endP.y << std::endl;
		}

		taskManager_->add(new Shot(shooter->getPosition(),endP,time_));
	}

	void ZombieGame::BeginContact(b2Contact* contact) {
		MovingObject* target;
		MovingObject* looker;
		if (getVisibleObject(contact, target, looker)) {
			looker->addVisibleObject(target);
		}
	}

	void ZombieGame::EndContact(b2Contact* contact) {
		MovingObject* target;
		MovingObject* looker;
		if (getVisibleObject(contact, target, looker)) {
			looker->removeVisibleObject(target);
		}
	}

	void ZombieGame::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) {
		if (std::abs(impulse->normalImpulses[0]) > 1.8f) {
			Object* ob1 = static_cast<Object*>(contact->GetFixtureA()->GetUserData());
			Object* ob2 = static_cast<Object*>(contact->GetFixtureB()->GetUserData());
			if (dynamic_cast<Car*>(ob1) || dynamic_cast<Car*>(ob2)) {
				mw::Sound tmp = crash;
				tmp.play();
			}
		}

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
