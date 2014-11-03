#include "zombieengine.h"
#include "box2ddef.h"
#include "gameinterface.h"
#include "unit.h"
#include "car.h"
#include "missile.h"
#include "building.h"
#include "weaponitem.h"
#include "closestraycastcallback.h"

namespace zombie {

	ZombieEngine::ZombieEngine(GameInterface& gameInterface, float impulseThreshold) : 
		gameInterface_(gameInterface), world_(b2Vec2(0, 0)), contactListener_(gameInterface, impulseThreshold) {
		
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

	void ZombieEngine::add(Unit* unit) {
		assert(dynamic_cast<Object*>(unit) != nullptr); // Test, Object type and not null.
		(static_cast<Object*>(unit))->createBody(&world_);
		unit->setActive(false);
		unit->setAwake(false);
		unit->getWeapon()->init(&world_, &gameInterface_);
		unit->setActionHandler(this);
	}

	void ZombieEngine::add(Car* car) {
		assert(dynamic_cast<Object*>(car) != nullptr); // Test, Object type and not null.
		(static_cast<Object*>(car))->createBody(&world_);
		car->setActive(false);
		car->setAwake(false);
		car->setActionHandler(this);
	}

	void ZombieEngine::add(Object* object) {
		assert(object != nullptr);
		object->createBody(&world_);
		object->setActive(false);
		object->setAwake(false);
	}

	void ZombieEngine::remove(Object* object) {
		assert(object != nullptr);
		object->destroyBody();
	}

	void ZombieEngine::unitEvent(Unit* unit, int eventType) {
		switch (eventType) {
			case Unit::ACTION:
				doAction(unit);
				break;
		}
	}

	void ZombieEngine::carEvent(Car* car, int eventType) {
		switch (eventType) {
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
			// Safe to do, because all userdata must be a Object!
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
