#include "zombieengine.h"
#include "gameinterface.h"
#include "unit.h"
#include "car.h"
#include "tree.h"
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
				closestFraction_ = 1.f;
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
					closestFraction_ = fraction;
				}

				// Fraction to clip the ray for closest hit.
				return fraction;
			}

			float getFraction() const {
				return closestFraction_;
			}

			void reset() {
				closestFraction_ = 1.f;
				fixture_ = nullptr;
			}

		private:
			b2Fixture* fixture_;
			std::function<bool(b2Fixture*)> conditionFunc_;
			float closestFraction_;
		};

	}

	ZombieEngine::ZombieEngine(GameInterface& gameInterface, int timeStepMS, float impulseThreshold) : gameInterface_(gameInterface), world_(b2Vec2(0, 0)), contactListener_(gameInterface, impulseThreshold) {
		impulseThreshold_ = impulseThreshold;
		human_ = nullptr;

		world_.SetContactListener(&contactListener_);

		started_ = false;
		time_ = 0.0f;

		timeStep_ = timeStepMS / 1000.f; // Fix time step for physics update.
		accumulator_ = 0.0f; // Time accumulator.
	}

	ZombieEngine::~ZombieEngine() {
		// Remove all game objects.
		std::vector<Object*> removeObjects;
		for (b2Body* b = world_.GetBodyList(); b; b = b->GetNext()) {
			Object* ob = static_cast<Object*>(b->GetUserData());
			// Can't remove in the loop because the iterator will be set in a undefined state.
			removeObjects.push_back(ob);
		}

		for (Object* ob : removeObjects) {
			delete ob;
		}
	}

	void ZombieEngine::start() {
		// Starts the game!
		started_ = true;
	}

	void ZombieEngine::updatePhysics(float timeStep) {
		// Game is started?
		if (started_) {
			if (human_ != nullptr) {
				gameInterface_.updateEachCycle(*human_);
			}

			// For all units except the human unit.
			if (human_ != nullptr) {
				for (Unit* unit : units_) {
					gameInterface_.updateEachCycle(*unit, *human_);
				}
			}

			// Update all game entities.
			for (b2Body* b = world_.GetBodyList(); b; b = b->GetNext()) {
				Object* ob = static_cast<Object*>(b->GetUserData());
				ob->update(time_, timeStep);
				ob->setHumanPosition(humanState_.position_);
				if (ob->toBeRemoved()) {
					remove(ob);
				}
			}

			// Update the objects physics interactions.
			world_.Step(timeStep, 6, 2);

			// Move the time ahead.
			time_ += timeStep;

			removeGarbage();
		}
	}

	void ZombieEngine::removeGarbage() {
		if (human_ != nullptr && human_->toBeRemoved()) {
			human_ = nullptr;
		}

		units_.remove_if([](Unit* unit) {
			return unit->toBeRemoved();
		});

		// Remove all players in a safe way. I.e. No risk of removing a player in current use.
		for (Player* player : garbagePlayers_) {
			delete player;
		}
		garbagePlayers_.clear();

		// Remove units in a safe way. I.e. No risk of removing a unit in current use.
		for (Object* object : garbageObjects_) {
			object->destroyBody(&world_);
			delete object;
		}
		garbageObjects_.clear();
	}

	void ZombieEngine::update(float frameTime) {
		if (frameTime > 0.25) {
			// To avoid spiral of death.
			frameTime = 0.25;
		}

		// Previous state for the human in the physic loop.
		State previousState;
		bool physicRan = false;

		accumulator_ += frameTime;
		while (accumulator_ >= timeStep_) {
			accumulator_ -= timeStep_;
			if (human_ != nullptr) {
				previousState = human_->getState();
				physicRan = true;
			}
			updatePhysics(timeStep_);
		}

		if (physicRan && human_ != nullptr && started_) {
			const float alpha = accumulator_ / timeStep_;
			humanState_ = human_->getState();
			humanState_.position_ = alpha * humanState_.position_ + (1.f - alpha) * previousState.position_;
			humanState_.velocity_ = alpha * humanState_.velocity_ + (1.f - alpha) * previousState.velocity_;
		}

		// Draw all objects.
		for (b2Body* b = world_.GetBodyList(); b; b = b->GetNext()) {
			Object* ob = static_cast<Object*>(b->GetUserData());
			ob->draw(accumulator_, timeStep_);
		}
	}

	void ZombieEngine::setHuman(DevicePtr device, State state, Unit* unit) {
		humanState_ = state;
		unit->createBody(&world_, state);
		human_ = unit;
		Player* player = new HumanPlayer(device, unit);
		unit->addEventHandler(std::bind(&ZombieEngine::unitEventHandler, this, unit, std::placeholders::_2));
	}

	void ZombieEngine::add(State state, Unit* unit) {
		unit->createBody(&world_, state);
		if (unit->isInfected()) {
			new ZombieBehavior(unit);
		} else {
			new SurvivorBehavior(unit);
		}
		unit->addEventHandler(std::bind(&ZombieEngine::unitEventHandler, this, unit, std::placeholders::_2));
		units_.push_back(unit);
	}

	void ZombieEngine::add(Building* building) {
		building->createBody(&world_);
	}

	void ZombieEngine::add(Tree* tree) {
		tree->createBody(&world_);
	}

	void ZombieEngine::add(State state, Car* car) {
		car->createBody(&world_, state);
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
		}
	}

	void ZombieEngine::carEventHandler(Car* car, Car::CarEvent carEvent) {
		switch (carEvent) {
			case Car::ACTION:
				doAction(car);
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
		world_.RayCast(&callback, unit->getPosition(), unit->getPosition() + dir);
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
		b2Vec2 hitPosition = shooter->getPosition() + bullet.range_ * dir;

		// Return the closest physcal object.
		ClosestRayCastCallback callback([](b2Fixture* fixture) {
			return !fixture->IsSensor();
		});

		world_.RayCast(&callback, shooter->getPosition(), hitPosition);
		b2Fixture* fixture = callback.getFixture();

		hitPosition = shooter->getPosition() + bullet.range_ * callback.getFraction() * dir;

		// Did bullet hit something?
		if (fixture != nullptr) {
			Object* ob = static_cast<Object*>(fixture->GetUserData());

			if (Unit* target = dynamic_cast<Unit*>(ob)) {
				gameInterface_.shotHit(bullet, hitPosition, *target);
				// Target alive?
				if (!target->isDead()) {
					target->updateHealthPoint(-bullet.damage_);
					// Died now?
					if (target->isDead()) {
						// Human?
						if (human_ == target) {
							gameInterface_.humanDied(*target);
						} else {
							gameInterface_.unitDied(*target);
						}
					}
				}
			} else {
				// Calculate the hit position on the unknown object.
				gameInterface_.shotMissed(bullet, hitPosition);
			}
		} else {
			gameInterface_.shotMissed(bullet, hitPosition);
		}
	}

} // Namespace zombie.
