#ifndef ZOMBIEENGINE_H
#define ZOMBIEENGINE_H

#include "object.h"
#include "state.h"
#include "device.h"
#include "input.h"
#include "typedefs.h"
#include "car.h"
#include "unit.h"

#include <mw/signal.h>
#include <Box2D/Box2D.h>
#include <SDL.h>

#include <memory>
#include <vector>
#include <string>

namespace zombie {

	// Forward declaration.
	class Weapon;
	class MovingObject;
	class Player;
	class Bullet;
	class GameInterface;

	// Responsible of all creation and deallocation of game objects 
	// and simulationg the game mechanics.
	class ZombieEngine : public b2ContactListener {
	public:
		ZombieEngine(GameInterface* gameInterface);
		~ZombieEngine();

		// Starts the game.
		void start();

		// Draws the graphic and (deltaTime) should be the time past 
		// from the previous call to this funtion.
		void update(float deltaTime);

		// Add a human player to the game.
		mw::signals::Connection setHuman(DevicePtr device, const State& state, float mass, float radius, float life, float walkingSpeed, float runningSpeed, const Weapon& weapon, std::function<void(Unit*, Unit::UnitEvent)> callback = [](Unit*, Unit::UnitEvent) {
		});

		// Add a ai player to the game.
		mw::signals::Connection addAi(const State& state, float mass, float radius, float life, float walkingSpeed, float runningSpeed, bool infected, const Weapon& weapon, std::function<void(Unit*, Unit::UnitEvent)> callback = [](Unit*, Unit::UnitEvent) {
		});

		// Add a car to the game.
		mw::signals::Connection addCar(const State& state, float mass, float life, float width, float length, std::function<void(Car*, Car::CarEvent)> callback = [](Car*, Car::CarEvent) {
		});

		// Add a building to the game.
		void addBuilding(const std::vector<Position>& corners);

		void addWeapon(float x, float y, const Weapon& weapon);

		// Get the current game time.
		inline float getTime() const {
			return time_;
		}

		inline bool isStarted() const {
			return started_;
		}

	private:
		// Creates a unit and add &doShotDamage to receive bullets fired.
		Unit* createUnit(const State& state, float mass, float radius, float life, float walkingSpeed, float runningSpeed, bool infected, const Weapon& weapon);

		Car* createCar(const State& state, float mass, float life, float width, float length);

		// Updates the game time by (msDeltaTime).
		void updatePhysics(float timeStep);

		void unitEventHandler(Unit* unit, Unit::UnitEvent unitEvent);
		void carEventHandler(Car* car, Car::CarEvent carEvent);

		void doAction(Unit* unit);
		void doAction(Car* unit);
		void doShotDamage(Unit* shooter, const Bullet& properties);

		void BeginContact (b2Contact* contact) override;
		void EndContact (b2Contact* contact) override;
		void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) override;

		void spawnAndCleanUpUnits(); // Spawns new units.

		bool started_; // The game is started.
		float time_; // Local game time.
		
		Unit* human_;
		std::list<Player*> players_;

		// Fix timestep.
		float timeStep_;
		float accumulator_;

		b2World* world_;
		GameInterface* gameInterface_;
	};

} // Namespace zombie.

#endif // ZOMBIEENGINE_H
