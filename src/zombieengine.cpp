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

	ZombieEngine::ZombieEngine(int width, int height) {
		// Create a world with no "gravity".
		world_ = new b2World(b2Vec2(0,0));
		Object::world = world_;
		worldHash_[0] = nullptr;
		Object::worldHash = &worldHash_;

		world_->SetContactListener(this);

		// Set windows size.
		updateSize(width,height);
		taskManager_ = new TaskManager();

		scale_ = 1.0;

		started_ = false;
		time_ = 0.0f;

		unitLevel_ = 20;

		timeStep_ = 0.017f; // Fix time step for physics update.
		accumulator_ = 0.0f; // Time accumulator.

		taskManager_->add(new SurvivalTimer(), GraphicLevel::INTERFACE_LEVEL);
	}

	ZombieEngine::~ZombieEngine() {
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

	mw::signals::Connection ZombieEngine::addEventListener(mw::Signal<const GameEvent&>::Callback callback) {
		return gameEventListener_.connect(callback);
	}

	mw::signals::Connection ZombieEngine::addRemoveListener(mw::Signal<bool&, const MovingObject*>::Callback callback) {
		return removeListener_.connect(callback);
	}

	void ZombieEngine::spawnAndCleanUpUnits() {
		// Delete all units outside the perimiter, and all the dead units.
		MovingObject* temp = nullptr;
		players_.remove_if([&](Player* player) {
			Object*& ob = worldHash_[player->getId()];
			if (ob != nullptr) {
				const MovingObject* mOb = static_cast<const MovingObject*>(ob);
				bool dead = mOb->isDead();

				if (dead) {
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

		// Draw map centered around first human player.
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

		glTranslated(-viewPosition_.x, -viewPosition_.y, 0.0);

		// Game is started?
		if (started_) {
			taskManager_->update(deltaTime);
		} else {
			taskManager_->update(0.0);
		}

		glPopMatrix();
	}

	void ZombieEngine::addHuman(DevicePtr device, float x, float y, float angle, float mass, float radius, float life, float walkingSpeed, float runningSpeed, const Weapon& weapon, const Animation& animation) {
		Unit* human = createUnit(x, y, angle, mass, radius, life, walkingSpeed, runningSpeed, false, weapon);
		viewPosition_ = human->getPosition();
		players_.push_back(new HumanPlayer(device, human));
		taskManager_->add(new HumanStatus(human, HumanStatus::ONE), GraphicLevel::INTERFACE_LEVEL);
		taskManager_->add(new HumanAnimation(human, animation), GraphicLevel::UNIT_LEVEL);
	}

	void ZombieEngine::addAi(float x, float y, float angle, float mass, float radius, float life, float walkingSpeed, float runningSpeed, bool infected, const Weapon& weapon, const Animation& animation) {
		Unit* unit = createUnit(x, y, angle, mass, radius, life, walkingSpeed, runningSpeed, infected, weapon);
		if (infected) {
			AiBehaviorPtr b = std::make_shared<ZombieBehavior>();
			taskManager_->add(new ZombieAnimation(unit, animation), GraphicLevel::UNIT_LEVEL);
			AiPlayer* aiPlayer(new AiPlayer(b, unit));
			players_.push_back(aiPlayer);
		} else {
			AiBehaviorPtr b = std::make_shared<SurvivorBehavior>();
			taskManager_->add(new HumanAnimation(unit, animation), GraphicLevel::UNIT_LEVEL);
			AiPlayer* aiPlayer(new AiPlayer(b, unit));
			players_.push_back(aiPlayer);
		}
	}

	void ZombieEngine::addCar(float x, float y, float angle, float mass, float life, float width, float length, const Animation& animation) {
		Car* car = createCar(x, y, angle, mass, life, width, length);
		taskManager_->add(new CarAnimation(car), GraphicLevel::UNIT_LEVEL);
	}

	void ZombieEngine::addBuilding(const std::vector<Position>& corners) {
		Building* building = new Building(corners);
		taskManager_->add(new BuildingDraw(building), GraphicLevel::BUILDING_LEVEL);
	}

	void ZombieEngine::addWeapon(float x, float y, const Weapon& weapon) {
		WeaponObject* wOb = new WeaponObject(x, y, weapon);
		worldHash_[wOb->getId()] = wOb;
		taskManager_->add(new DrawWeaponObject(wOb), GraphicLevel::TREE_LEVEL);
	}

	void ZombieEngine::addGrassGround(float minX, float maxX, float minY, float maxY) {
		taskManager_->add(new MapDraw(minX,maxX,minY,maxY), GraphicLevel::GROUND_LEVEL);
	}

	Position ZombieEngine::getMainUnitPostion() {
		const MovingObject* frontOb = static_cast<const MovingObject*>(worldHash_[players_.front()->getId()]);
		if (frontOb != nullptr) {
			return frontOb->getPosition();
		}
		return Position();
	}

	Unit* ZombieEngine::createUnit(float x, float y, float angle, float mass, float radius, float life, float walkingSpeed, float runningSpeed, bool infected, const Weapon& weapon) {
		Unit* unit = new Unit(x, y, angle, mass, radius, life, walkingSpeed, runningSpeed, infected, weapon);
		unit->addShootHandler(std::bind(&ZombieEngine::doShotDamage,this,std::placeholders::_1,std::placeholders::_2));
		unit->addActionHandler(std::bind(&ZombieEngine::doAction,this,std::placeholders::_1));
		worldHash_[unit->getId()] = unit;
		return unit;
	}

	Car* ZombieEngine::createCar(float x, float y, float angle, float mass, float life, float width, float length) {
		Car* car = new Car(x, y, angle, mass, life, width, length);
		car->addActionHandler(std::bind(&ZombieEngine::carDoAction,this,std::placeholders::_1));
		worldHash_[car->getId()] = car;
		cars_.push_back(car);
		return car;
	}

	void ZombieEngine::zoom(double scale) {
		scale_ *= scale;
	}

	void ZombieEngine::updateSize(int width, int height) {
		Task::width = width;
		Task::height = height;
	}

	void ZombieEngine::doAction(Unit* unit) {
		float angle = unit->getState().angle_;
		b2Vec2 dir(std::cos(angle),std::sin(angle));

		// Return the closest object, physical or not.
		ClosestRayCastCallback callback( [](b2Fixture* fixture) {
			return !fixture->IsSensor() && fixture->GetBody()->GetUserData() != nullptr || fixture->IsSensor() && fixture->GetUserData() != nullptr;
		});
		world_->RayCast(&callback,unit->getPosition(),unit->getPosition() + dir);
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
				body->SetTransform(car->getPosition(),body->GetAngle());
				return true;
			}
			return false;
		});
	}

	void ZombieEngine::doShotDamage(Unit* shooter, const Bullet& bullet) {
		b2Vec2 dir(std::cos(bullet.direction_),std::sin(bullet.direction_));
		b2Vec2 endP = shooter->getPosition() + bullet.range_ * dir;

		// Return the closest physcal object.
		ClosestRayCastCallback callback([](b2Fixture* fixture) {
			return !fixture->IsSensor();
		});

		world_->RayCast(&callback,shooter->getPosition(),endP);
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
						taskManager_->add(new Blood(endP.x, endP.y, time_), GraphicLevel::BLOOD_LEVEL);
						taskManager_->add(new BloodStain(endP.x, endP.y, time_), GraphicLevel::BLOOD_LEVEL);
					} else {
						taskManager_->add(new BloodSplash(endP.x, endP.y, time_), GraphicLevel::BLOOD_LEVEL);
					}
				}
			}
		} else {
			//std::cout << endP.x << " " << endP.y << std::endl;
		}

		taskManager_->add(new Shot(shooter->getPosition(),endP,time_), GraphicLevel::SHOT_LEVEL);
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

	void ZombieEngine::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) {
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
					taskManager_->add(new Blood(p.x,p.y,time_), GraphicLevel::BLOOD_LEVEL);
					taskManager_->add(new BloodStain(p.x,p.y,time_), GraphicLevel::BLOOD_LEVEL);
				} else {
					taskManager_->add(new BloodSplash(p.x,p.y,time_), GraphicLevel::BLOOD_LEVEL);
				}
			}

			Object* ob2 = static_cast<Object*>(contact->GetFixtureB()->GetUserData());
			if (Unit* unit = dynamic_cast<Unit*>(ob2)) {
				unit->updateHealthPoint(-101.0);
				Position p = unit->getPosition();
				if (unit->isDead()) {
					taskManager_->add(new Blood(p.x,p.y,time_), GraphicLevel::BLOOD_LEVEL);
					taskManager_->add(new BloodStain(p.x,p.y,time_), GraphicLevel::BLOOD_LEVEL);
				} else {
					taskManager_->add(new BloodSplash(p.x,p.y,time_), GraphicLevel::BLOOD_LEVEL);
				}
			}
		}
	}

} // Namespace zombie.
