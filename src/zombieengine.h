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
	class Building;
	class Unit;
	class WeaponItem;

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
		void setHuman(DevicePtr device, Unit* unit);

		// Add a ai player to the game.
		void addAi(Unit* unit);

		// Get the current game time.
		inline float getTime() const {
			return time_;
		}

		inline bool isStarted() const {
			return started_;
		}

		inline b2World* getWorld() {
			return world_;
		}

	private:
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
