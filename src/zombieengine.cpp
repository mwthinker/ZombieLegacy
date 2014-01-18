#include "zombieengine.h"
#include "gameinterface.h"
#include "typedefs.h"
#include "unit.h"
#include "building.h"
#include "bullet.h"
#include "input.h"
#include "zombiebehavior.h"
#include "survivorbehavior.h"
#include "car.h"
#include "humanplayer.h"
#include "aiplayer.h"
#include "weaponitem.h"

#include <Box2D/Box2D.h>

#include <memory>
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

			Object* ob1 = static_cast<Object*>(fixtureA->GetBody()->GetUserData());
			Object* ob2 = static_cast<Object*>(fixtureB->GetBody()->GetUserData());
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

	ZombieEngine::ZombieEngine(GameInterface* gameInterface) {
		gameInterface_ = gameInterface;
		human_ = nullptr;

		// Create a world with no "gravity".
		world_ = new b2World(b2Vec2(0, 0));

		world_->SetContactListener(this);

		started_ = false;
		time_ = 0.0f;

		timeStep_ = 0.017f; // Fix time step for physics update.
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
			delete ob->getPlayer();
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
			spawnAndCleanUpUnits();

			// Update all game entities.
			for (Player* player : players_) {
				player->updatePhysics(time_, timeStep);
			}

			// Update the objects physics interactions.
			world_->Step(timeStep, 4, 2);

			// Move the time ahead.
			time_ += timeStep;

			if (human_ != nullptr) {
				Position p = human_->getPosition();
				gameInterface_->humanPosition(p.x, p.y);
			}
		}
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

	void ZombieEngine::spawnAndCleanUpUnits() {
		// Delete all units outside the perimiter, and all the dead units.
		players_.remove_if([&](Player* player) {
			MovingObject* mOb = player->getMovingObject();
			if (mOb != nullptr) {
				Position p = mOb->getPosition();
				if (mOb->isDead() || gameInterface_->isUnitOutside(p.x, p.y, mOb->isInfected())) {
					// Human died?
					if (human_ != nullptr && human_->getId() == mOb->getId()) {
						gameInterface_->humanDied();
						human_ = nullptr;
					} else { // A ai died!
						gameInterface_->unitDied(p.x, p.y, mOb->isInfected());
					}

					delete player;
					delete mOb;

					// Removes the pointer from the list.
					return true;
				}
			} else {
				//delete player;
				//delete mOb;
			}

			// The pointer is not removed.
			return false;
		});
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

		for (Player* player : players_) {
			player->getMovingObject()->callUpdateHandler(accumulator_);
		}

		for (b2Body* b = world_->GetBodyList(); b; b = b->GetNext()) {
			Object* ob = static_cast<Object*>(b->GetUserData());
			ob->draw(deltaTime);
		}
	}

	void ZombieEngine::setHuman(DevicePtr device, Unit* unit) {
		human_ = unit;
		Player* player = new HumanPlayer(device, unit);
		players_.push_back(player);
	}

	void ZombieEngine::addAi(Unit* unit) {
		if (unit->isInfected()) {
			AiBehaviorPtr behavior(new ZombieBehavior);
			Player* player = new AiPlayer(behavior, unit);
			players_.push_back(player);
		} else {
			AiBehaviorPtr behavior(new SurvivorBehavior);
			Player* player = new AiPlayer(behavior, unit);
			players_.push_back(player);
		}
	}

	void ZombieEngine::doAction(Unit* unit) {
		float angle = unit->getState().angle_;
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
				// Driver seat is empty?
				if (car->getDriver() == nullptr) {
					/*
					// The unit gets into the car.
					std::swap(car->gameEntity_->player_, unit->gameEntity_->player_);
					car->setDriver(unit);
					unit->getBody()->SetActive(false);
					*/
				}
			} else if (WeaponItem* wItem = dynamic_cast<WeaponItem*>(ob)) {
				// Change the weapon.
				unit->setWeapon(wItem->getWeapon());
				delete wItem;
			}
		}
	}

	void ZombieEngine::doAction(Car* car) {
		Unit* driver = car->getDriver();
		/*
		// The driver gets out of the car.
		std::swap(car->gameEntity_->player_, driver->gameEntity_->player_);
		car->setDriver(nullptr);
		b2Body* driverBody = driver->getBody();
		driverBody->SetActive(true);
		driverBody->SetTransform(car->getPosition(), car->getDirection());
		*/
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
				// Target alive?
				if (!target->isDead()) {
					target->updateHealthPoint(-bullet.damage_);
				}
			}
		}
		gameInterface_->shotFired(bullet);
	}

	void ZombieEngine::BeginContact(b2Contact* contact) {
		MovingObject* object;
		MovingObject* looker;
		if (getVisibleObject(contact, object, looker)) {
			looker->addSeenObject(object);
		}
	}

	// Is called when the contact has get out of view or is destroyed.
	void ZombieEngine::EndContact(b2Contact* contact) {
		MovingObject* object;
		MovingObject* looker;
		if (getVisibleObject(contact, object, looker)) {
			looker->removeSeenObject(object);
		}
	}

	namespace {

		void unitCollision(GameInterface* gameInterface, float time, Object* ob, const b2ContactImpulse* impulse) {
			if (MovingObject* mOv = dynamic_cast<Car*>(ob)) {
				mOv->collision(std::abs(impulse->normalImpulses[0]));
				// Is a car?
				if (dynamic_cast<Car*>(ob)) {
					gameInterface->carCollision();
				} else if (Unit* unit = dynamic_cast<Unit*>(ob)) { // Is a unit?
					gameInterface->unitCollision();
				}
			}
		}
	}

	void ZombieEngine::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) {
		Object* ob1 = static_cast<Object*>(contact->GetFixtureA()->GetUserData());
		unitCollision(gameInterface_, time_, ob1, impulse);
		Object* ob2 = static_cast<Object*>(contact->GetFixtureA()->GetUserData());
		unitCollision(gameInterface_, time_, ob2, impulse);
	}

} // Namespace zombie.
