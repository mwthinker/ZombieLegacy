#include "zombieengine.h"
#include "gameinterface.h"
#include "unit.h"
#include "car.h"
#include "building.h"
#include "weaponitem.h"
#include "bullet.h"
#include "input.h"
#include "zombiebehavior.h"
#include "survivorbehavior.h"
#include "humanplayer.h"
#include "driver.h"
#include "box2ddef.h"

#include <vector>
#include <string>

namespace zombie {

	namespace {

		void collision(GameInterface* game, float impulse, Object* ob1, Object* ob2) {
			if (Car* car1 = dynamic_cast<Car*>(ob1)) {
				if (Car* car2 = dynamic_cast<Car*>(ob2)) {
					game->collision(impulse, *car1, *car2);
				} else if (Unit* unit2 = dynamic_cast<Unit*>(ob2)) {
					game->collision(impulse, *car1, *unit2);
				} else if (Building* building = dynamic_cast<Building*>(ob2)) {
					game->collision(impulse, *car1, *building);
				}
			} else if (Unit* unit1 = dynamic_cast<Unit*>(ob1)) {
				if (nullptr != dynamic_cast<Unit*>(ob2)) {
					// ob2 is not a unit, this in order to avoid a infinity loop.
					collision(game, impulse, ob2, ob1);
				}
			}
		}

		class InViewQueryCallback : public b2QueryCallback {
		public:
			std::vector<b2Fixture*> foundFixtures;

			bool ReportFixture(b2Fixture* fixture) {
				foundFixtures.push_back(fixture);
				return true; // Keep going to find all fixtures in the query area.
			}

			void reset() {
				foundFixtures.clear();
			}
		};

		class ClosestRayCastCallback : public b2RayCastCallback {
		public:
			ClosestRayCastCallback(std::function<bool(b2Fixture*)> conditionFunc) {
				closestFraction_ = 100.f;
				fixture_ = nullptr;
				conditionFunc_ = conditionFunc;
			}

			b2Fixture* getFixture() const {
				return fixture_;
			}

			// Ray-cast callback.
			float32 ReportFixture(b2Fixture* fixture, const b2Vec2 &point, const b2Vec2 &normal, float32 fraction) override {
				// Is a physical fixture?
				if (conditionFunc_(fixture)) {
					fixture_ = fixture;
				}

				// Fraction to clip the ray for closest hit.
				return fraction;
			}

			void reset() {
				closestFraction_ = 100.f;
				fixture_ = nullptr;
			}

		private:
			b2Fixture* fixture_;
			std::function<bool(b2Fixture*)> conditionFunc_;
			float closestFraction_;
		};

		bool getVisibleObject(b2Contact* contact, MovingObject*& target, MovingObject*& looker) {
			b2Fixture* fixtureA = contact->GetFixtureA();
			b2Fixture* fixtureB = contact->GetFixtureB();

			// Make sure only one of the fixtures was a sensor.
			bool sensorA = fixtureA->IsSensor();
			bool sensorB = fixtureB->IsSensor();
			if (sensorA == sensorB) {
				return false;
			}

			Object* obA = static_cast<Object*>(fixtureA->GetBody()->GetUserData());
			Object* obB = static_cast<Object*>(fixtureB->GetBody()->GetUserData());
			MovingObject* mObA = dynamic_cast<MovingObject*>(obA);
			MovingObject* mObB = dynamic_cast<MovingObject*>(obB);

			// Make sure both are moving objects.
			if (mObA != nullptr && mObB != nullptr && mObA && mObB) {
				if (sensorA) {
					looker = mObA;
					target = mObB;
				} else {
					looker = mObB;
					target = mObA;
				}
				return true;
			}

			return false;
		}
	}

	ZombieEngine::ZombieEngine(GameInterface* gameInterface, int timeStepMS, float impulseThreshold) {
		gameInterface_ = gameInterface;
		impulseThreshold_ = impulseThreshold;
		human_ = nullptr;

		// Create a world with no "gravity".
		world_ = new b2World(b2Vec2(0, 0));

		world_->SetContactListener(this);

		started_ = false;
		time_ = 0.0f;

		timeStep_ = timeStepMS / 1000.f; // Fix time step for physics update.
		accumulator_ = 0.0f; // Time accumulator.
	}

	ZombieEngine::~ZombieEngine() {
		// Remove all game objects.
		std::vector<Object*> removeObjects;
		for (b2Body* b = world_->GetBodyList(); b; b = b->GetNext()) {
			Object* ob = static_cast<Object*>(b->GetUserData());
			// Can't remove in the loop because the iterator will be set in a undefined state.
			removeObjects.push_back(ob);
		}

		for (Object* ob : removeObjects) {
			delete ob;
		}

		// All game objects are removed, remove world!
		delete world_;
	}

	void ZombieEngine::start() {
		// Starts the game!
		started_ = true;
	}

	void ZombieEngine::updatePhysics(float timeStep) {
		// Game is started?
		if (started_) {
			if (human_ != nullptr) {
				gameInterface_->updateSpawning(*human_);
			}

			// Update all game entities.
			for (b2Body* b = world_->GetBodyList(); b; b = b->GetNext()) {
				Object* ob = static_cast<Object*>(b->GetUserData());
				ob->update(time_, timeStep);
				if (ob->toBeRemoved()) {
					remove(ob);
				}
			}

			// Update the objects physics interactions.
			world_->Step(timeStep, 4, 2);

			// Move the time ahead.
			time_ += timeStep;

			if (human_ != nullptr && !human_->toBeRemoved()) {
				Position p = human_->getPosition();
				gameInterface_->currentHuman(*human_);
			} else {
				human_ = nullptr;
			}

			removeGarbage();
		}
	}

	void ZombieEngine::removeGarbage() {
		// Remove all players in a safe way. I.e. No risk of removing a player in current use.
		for (Player* player : garbagePlayers_) {
			delete player;
		}
		garbagePlayers_.clear();

		// Remove units in a safe way. I.e. No risk of removing a unit in current use.
		for (Object* object : garbageObjects_) {
			object->destroyBody(world_);
			delete object;
		}
		garbageObjects_.clear();
	}

	void ZombieEngine::update(float deltaTime) {
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

		// Draw all objects.
		for (b2Body* b = world_->GetBodyList(); b; b = b->GetNext()) {
			Object* ob = static_cast<Object*>(b->GetUserData());
			ob->draw(deltaTime);
		}
	}

	void ZombieEngine::setHuman(DevicePtr device, State state, Unit* unit) {
		unit->createBody(world_, state);
		human_ = unit;
		Player* player = new HumanPlayer(device, unit);
		unit->addEventHandler(std::bind(&ZombieEngine::unitEventHandler, this, unit, std::placeholders::_2));
	}

	void ZombieEngine::add(State state, Unit* unit) {
		unit->createBody(world_, state);
		if (unit->isInfected()) {
			new ZombieBehavior(unit);
		} else {
			new SurvivorBehavior(unit);
		}
		unit->addEventHandler(std::bind(&ZombieEngine::unitEventHandler, this, unit, std::placeholders::_2));
	}

	void ZombieEngine::add(Building* building) {
		building->createBody(world_);
	}

	void ZombieEngine::add(State state, Car* car) {
		car->createBody(world_, state);
		car->addEventHandler(std::bind(&ZombieEngine::carEventHandler, this, car, std::placeholders::_2));
	}

	void ZombieEngine::unitEventHandler(Unit* unit, Unit::UnitEvent unitEvent) {
		switch (unitEvent) {
			case Unit::ACTION:
				doAction(unit);
				break;
			case Unit::SHOOT:
				doShotDamage(unit, unit->getLastBullet());
				break;
			default:
				break;
		}
	}

	void ZombieEngine::carEventHandler(Car* car, Car::CarEvent carEvent) {
		switch (carEvent) {
			case Car::ACTION:
				doAction(car);
				break;
			default:
				break;
		}
	}

	void ZombieEngine::doAction(Unit* unit) {
		float angle = unit->getDirection();
		b2Vec2 dir(std::cos(angle), std::sin(angle));

		// Return the closest object, physical or not.
		ClosestRayCastCallback callback([](b2Fixture* fixture) {
			return !fixture->IsSensor() && fixture->GetBody()->GetUserData() != nullptr || fixture->IsSensor() && fixture->GetUserData() != nullptr;
		});
		world_->RayCast(&callback, unit->getPosition(), unit->getPosition() + dir);
		b2Fixture* fixture = callback.getFixture();

		// Is there an object near by?
		if (fixture != nullptr) {
			Object* ob = static_cast<Object*>(fixture->GetUserData());

			if (Car* car = dynamic_cast<Car*>(ob)) {
				// Car is empty?
				if (car->getDriver() == nullptr) {
					Driver* driver = unit->getPlayer()->createDriver(car);
					// The player is able to drive?
					if (driver != nullptr) {
						car->setDriver(driver);
						unit->setActive(false);
					}
				}
			} else if (WeaponItem* wItem = dynamic_cast<WeaponItem*>(ob)) {
				// Change the weapon.
				unit->setWeapon(wItem->getWeapon());
				remove(wItem);
			}
		}
	}

	void ZombieEngine::doAction(Car* car) {
		Driver* driver = car->getDriver();
		driver->getUnit()->setActive(true);
		garbagePlayers_.push_back(driver);
		car->setDriver(nullptr);
	}

	void ZombieEngine::doShotDamage(Unit* shooter, const Bullet& bullet) {
		b2Vec2 dir(std::cos(bullet.direction_), std::sin(bullet.direction_));
		b2Vec2 endP = shooter->getPosition() + bullet.range_ * dir;

		// Return the closest physcal object.
		ClosestRayCastCallback callback([](b2Fixture* fixture) {
			return !fixture->IsSensor();
		});

		world_->RayCast(&callback, shooter->getPosition(), endP);
		b2Fixture* fixture = callback.getFixture();

		// Did bullet hit something?
		if (fixture != nullptr) {
			Object* ob = static_cast<Object*>(fixture->GetUserData());

			if (Unit* target = dynamic_cast<Unit*>(ob)) {
				gameInterface_->shotHit(bullet, *target);
				// Target alive?
				if (!target->isDead()) {
					target->updateHealthPoint(-bullet.damage_);
					// Died now?
					if (target->isDead()) {
						// Human?
						if (human_ == target) {
							gameInterface_->humanDied(*target);
						} else {
							gameInterface_->unitDied(*target);
						}
					}
				}
			} else {
				gameInterface_->shotMissed(bullet);
			}
		}
	}

	void ZombieEngine::BeginContact(b2Contact* contact) {
		MovingObject* object;
		MovingObject* looker;
		if (getVisibleObject(contact, object, looker)) {
			looker->addSeenObject(object);
		}
	}

	void ZombieEngine::EndContact(b2Contact* contact) {
		MovingObject* object;
		MovingObject* looker;
		if (getVisibleObject(contact, object, looker)) {
			looker->removeSeenObject(object);
		}
	}	

	void ZombieEngine::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) {
		Object* ob1 = static_cast<Object*>(contact->GetFixtureA()->GetUserData());
		Object* ob2 = static_cast<Object*>(contact->GetFixtureB()->GetUserData());
		float maxImpulse = 0;
		for (int32 i = 0; i < impulse->count; ++i) {
			maxImpulse = b2Max(maxImpulse, impulse->normalImpulses[i]);
		}

		if (maxImpulse > impulseThreshold_) {
			collision(gameInterface_, maxImpulse, ob1, ob2);
		}
	}

} // Namespace zombie.
