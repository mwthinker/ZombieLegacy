#include "zombieengine.h"
#include "typedefs.h"
#include "unit.h"
#include "building.h"
#include "bullet.h"
#include "input.h"
#include "weaponobject.h"
#include "roaddraw.h"
#include "mapdraw.h"
#include "drawweaponobject.h"
#include "buildingdraw.h"
#include "humanstatus.h"
#include "sky.h"
#include "zombiebehavior.h"
#include "survivorbehavior.h"
#include "gamesound.h"
#include "taskmanager.h"
#include "aiplayer.h"
#include "survivaltimer.h"
#include "car.h"
#include "blood.h"
#include "caranimation.h"
#include "zombieanimation.h"
#include "humananimation.h"
#include "humanplayer.h"
#include "shot.h"
#include "animation.h"
#include "gameinterface.h"
#include "buildingproperties.h"
#include "weaponproperties.h"
#include "unitproperties.h"
#include "carproperties.h"

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
		Object::world = world_;
		worldHash_[0] = nullptr;
		Object::worldHash = &worldHash_;

		world_->SetContactListener(this);

		started_ = false;
		time_ = 0.0f;

		timeStep_ = 0.017f; // Fix time step for physics update.
		accumulator_ = 0.0f; // Time accumulator.

		taskManager_.add(new SurvivalTimer(), GraphicLevel::INTERFACE_LEVEL);
	}

	ZombieEngine::~ZombieEngine() {
		// Remove all game objects.
		for (b2Body* b = world_->GetBodyList(); b; b = b->GetNext()) {
			Object* ob = static_cast<Object*>(b->GetUserData());
			delete ob;
		}

		// When all game objects are removed then remove world.
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

			// Update all cars without drivers.
			for (Car* car : cars_) {
				if (car->getDriverId() == 0) {
					car->updatePhysics(time_, timeStep, Input());
				}
			}

			// Update all players.
			for (Player* player : players_) {
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

			if (human_ != nullptr) {
				Position p = human_->getPosition();
				gameInterface_->humanPosition(p.x, p.y);
			}
		}
	}

	void ZombieEngine::spawnAndCleanUpUnits() {
		// Delete all units outside the perimiter, and all the dead units.
		players_.remove_if([&](Player* player) {
			Object*& ob = worldHash_[player->getId()];
			if (ob != nullptr) {
				const MovingObject* mOb = static_cast<const MovingObject*>(ob);
				Position p = mOb->getPosition();
				if (mOb->isDead() || gameInterface_->isUnitOutside(p.x, p.y, mOb->isInfected())) {
					// Human died?
					if (human_ != nullptr && human_->getId() == mOb->getId()) {
						gameInterface_->humanDies();
						human_ = nullptr;
					} else { // A ai died!
						gameInterface_->unitDie(p.x, p.y, mOb->isInfected());
					}

					delete player;
					delete ob;
					// Hash to null.
					ob = nullptr;

					// Removes the pointer from the list.
					return true;
				}
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
	}

	void ZombieEngine::draw(float deltaTime) {
		// Game is started?
		if (started_) {
			taskManager_.update(deltaTime);
		} else {
			taskManager_.update(0.0);
		}
	}

	void ZombieEngine::setHuman(DevicePtr device, const State& state, float mass, float radius, float life, float walkingSpeed, float runningSpeed, const Weapon& weapon, const Animation& animation) {
		if (human_ != nullptr) {
			worldHash_[human_->getId()] = nullptr;
			delete human_;
		}
		human_ = createUnit(state, mass, radius, life, walkingSpeed, runningSpeed, false, weapon);
		players_.push_back(new HumanPlayer(device, human_));
		taskManager_.add(new HumanStatus(human_, HumanStatus::ONE), GraphicLevel::INTERFACE_LEVEL);
		taskManager_.add(new HumanAnimation(human_, animation), GraphicLevel::UNIT_LEVEL);
	}

	void ZombieEngine::addAi(const State& state, float mass, float radius, float life, float walkingSpeed, float runningSpeed, bool infected, const Weapon& weapon, const Animation& animation) {
		Unit* unit = createUnit(state, mass, radius, life, walkingSpeed, runningSpeed, infected, weapon);
		if (infected) {
			AiBehaviorPtr b = std::make_shared<ZombieBehavior>();
			taskManager_.add(new ZombieAnimation(unit, animation), GraphicLevel::UNIT_LEVEL);
			AiPlayer* aiPlayer(new AiPlayer(b, unit));
			players_.push_back(aiPlayer);
		} else {
			AiBehaviorPtr b = std::make_shared<SurvivorBehavior>();
			taskManager_.add(new HumanAnimation(unit, animation), GraphicLevel::UNIT_LEVEL);
			AiPlayer* aiPlayer(new AiPlayer(b, unit));
			players_.push_back(aiPlayer);
		}
	}

	void ZombieEngine::addCar(const State& state, float mass, float life, float width, float length, const Animation& animation) {
		Car* car = createCar(state, mass, life, width, length);
		taskManager_.add(new CarAnimation(car), GraphicLevel::UNIT_LEVEL);
	}

	void ZombieEngine::addBuilding(const std::vector<Position>& corners) {
		Building* building = new Building(corners);
		taskManager_.add(new BuildingDraw(building), GraphicLevel::BUILDING_LEVEL);
	}

	void ZombieEngine::addWeapon(float x, float y, const Weapon& weapon) {
		WeaponObject* wOb = new WeaponObject(x, y, weapon);
		worldHash_[wOb->getId()] = wOb;
		taskManager_.add(new DrawWeaponObject(wOb), GraphicLevel::TREE_LEVEL);
	}

	void ZombieEngine::addGrassGround(float minX, float maxX, float minY, float maxY) {
		taskManager_.add(new MapDraw(minX, maxX, minY, maxY), GraphicLevel::GROUND_LEVEL);
	}

	Unit* ZombieEngine::createUnit(const State& state, float mass, float radius, float life, float walkingSpeed, float runningSpeed, bool infected, const Weapon& weapon) {
		Unit* unit = new Unit(state, mass, radius, life, walkingSpeed, runningSpeed, infected, weapon);
		unit->addShootHandler(std::bind(&ZombieEngine::doShotDamage, this, std::placeholders::_1, std::placeholders::_2));
		unit->addActionHandler(std::bind(&ZombieEngine::doAction, this, std::placeholders::_1));
		worldHash_[unit->getId()] = unit;
		return unit;
	}

	Car* ZombieEngine::createCar(const State& state, float mass, float life, float width, float length) {
		Car* car = new Car(state, mass, life, width, length);
		car->addActionHandler(std::bind(&ZombieEngine::carDoAction, this, std::placeholders::_1));
		worldHash_[car->getId()] = car;
		cars_.push_back(car);
		return car;
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
				if (car->getDriverId() == 0) {
					// The unit gets into the car.
					std::find_if(players_.begin(), players_.end(), [car, unit](Player* player) {
						if (unit->getId() == player->getId()) {
							player->id_ = car->getId();
							car->setDriver(unit);
							unit->getBody()->SetActive(false);
							return true;
						}
						return false;
					});
				}
			} else if (WeaponObject* wOb = dynamic_cast<WeaponObject*>(ob)) {
				// Change the weapon.
				unit->setWeapon(wOb->getWeapon());
				worldHash_[wOb->getId()] = nullptr;
				delete wOb;
			}
		}
	}

	void ZombieEngine::carDoAction(Car* car) {
		Unit* driver = static_cast<Unit*>(worldHash_[car->getDriverId()]);

		// The driver gets out of the car.
		std::find_if(players_.begin(), players_.end(), [driver, car](Player* player) {
			if (car->getId() == player->getId()) {
				player->id_ = driver->getId();
				car->setDriver(0);
				b2Body* body = driver->getBody();
				body->SetActive(true);
				body->SetTransform(car->getPosition(), body->GetAngle());
				return true;
			}
			return false;
		});
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
					endP = target->getPosition();
					// Target killed?
					if (target->isDead()) {
						taskManager_.add(new Blood(endP.x, endP.y, time_), GraphicLevel::BLOOD_LEVEL);
						taskManager_.add(new BloodStain(endP.x, endP.y, time_), GraphicLevel::BLOOD_LEVEL);
					} else {
						taskManager_.add(new BloodSplash(endP.x, endP.y, time_), GraphicLevel::BLOOD_LEVEL);
					}
				}
			}
		} else {
			//std::cout << endP.x << " " << endP.y << std::endl;
		}

		taskManager_.add(new Shot(shooter->getPosition(), endP, time_), GraphicLevel::SHOT_LEVEL);
	}

	void ZombieEngine::BeginContact(b2Contact* contact) {
		MovingObject* target;
		MovingObject* looker;
		if (getVisibleObject(contact, target, looker)) {
			looker->addVisibleObject(target);
		}
	}

	void ZombieEngine::EndContact(b2Contact* contact) {
		MovingObject* target;
		MovingObject* looker;
		if (getVisibleObject(contact, target, looker)) {
			looker->removeVisibleObject(target);
		}
	}

	namespace {

		void unitCollision(TaskManager& taskManager, float time, Object* ob, const b2ContactImpulse* impulse) {
			if (MovingObject* mOv = dynamic_cast<Car*>(ob)) {
				bool cEvent = mOv->collision(std::abs(impulse->normalImpulses[0]));
				if (cEvent) {
					if (dynamic_cast<Car*>(ob)) {
						mw::Sound tmp = crash;
						tmp.play();
					} else if (Unit* unit = dynamic_cast<Unit*>(ob)) {
						Position p = unit->getPosition();
						if (unit->isDead()) {
							taskManager.add(new Blood(p.x, p.y, time), GraphicLevel::BLOOD_LEVEL);
							taskManager.add(new BloodStain(p.x, p.y, time), GraphicLevel::BLOOD_LEVEL);
						} else {
							taskManager.add(new BloodSplash(p.x, p.y, time), GraphicLevel::BLOOD_LEVEL);
						}
					}
				}
			}
		}

	}

	void ZombieEngine::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) {
		Object* ob1 = static_cast<Object*>(contact->GetFixtureA()->GetUserData());
		unitCollision(taskManager_, time_, ob1, impulse);
		Object* ob2 = static_cast<Object*>(contact->GetFixtureA()->GetUserData());
		unitCollision(taskManager_, time_, ob2, impulse);
	}

} // Namespace zombie.
