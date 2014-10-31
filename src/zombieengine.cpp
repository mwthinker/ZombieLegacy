#include "zombieengine.h"
#include "box2ddef.h"
#include "gameinterface.h"
#include "unit.h"
#include "car.h"
#include "weaponitem.h"
#include "humanplayer.h"
#include "closestraycastcallback.h"

namespace zombie {

	ZombieEngine::ZombieEngine(GameInterface& gameInterface, float impulseThreshold) : gameInterface_(gameInterface), world_(b2Vec2(0, 0)), contactListener_(gameInterface, impulseThreshold) {
		world_.SetContactListener(&contactListener_);
		started_ = false;
		time_ = 0.0f;
	}

	ZombieEngine::~ZombieEngine() {
	}

	void ZombieEngine::update(float timeStep) {
		// Update the objects physics interactions.
		world_.Step(timeStep, 6, 2);
		
		// Move the time ahead.
		time_ += timeStep;
	}

	void ZombieEngine::add(State state, Unit* unit) {
		unit->createBody(&world_, state);
		unit->getWeapon()->init(&world_, &gameInterface_);
		unit->addEventHandler(std::bind(&ZombieEngine::unitEventHandler, this, unit, std::placeholders::_2));
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
					// The player is able to drive?
					car->setDriver(unit);
					unit->setActive(false);
				}
			} else if (WeaponItem* wItem = dynamic_cast<WeaponItem*>(ob)) {
				// Change the weapon.
				unit->setWeapon(wItem->getWeapon());
				//remove(wItem);
			}
		}
	}

	void ZombieEngine::doAction(Car* car) {
		Unit* driver = car->getDriver();
		driver->setActive(true);
		car->setDriver(nullptr);
	}

} // Namespace zombie.
