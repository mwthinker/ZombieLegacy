#ifndef ZOMBIEENGINE_H
#define ZOMBIEENGINE_H

#include "device.h"
#include "unit.h"
#include "car.h"
#include "box2ddef.h"
#include "missile.h"

#include <mw/signal.h>

#include <vector>
#include <string>
#include <list>

namespace zombie {

	// Forward declarations.
	class Weapon;
	class MovingObject;
	class Player;
	class Bullet;
	class GameInterface;
	class Building;
	class WeaponItem;

	// Responsible of all creation and deallocation of game objects 
	// and simulating the game mechanics.
	class ZombieEngine : public b2ContactListener {
	public:
		ZombieEngine(GameInterface& gameInterface, int timeStepMS, float impulseThreshold);
		~ZombieEngine();

		// Starts the game.
		void start();

		// Draws the graphic and (deltaTime) should be the time past 
		// from the previous call to this funtion.
		void update(float deltaTime);

		// Add a human player to the game.
		void setHuman(DevicePtr device, State states, Unit* unit);

		// Add a ai player to the game.
		void add(State state, Unit* unit);

		// Add a empty car to the game.
		void add(State state, Car* car);

		void add(Building* building);

		// Get the current game time.
		inline float getTime() const {
			return time_;
		}

		// Return whether the game is started or not.
		inline bool isStarted() const {
			return started_;
		}
		
		// Returns the current world. Use with care!
		inline b2World* getWorld() {
			return world_;
		}

		// Removes the object from memory in a safe way.
		// Not removed immediately. The removal is performed 
		// in the end of every physics update.
		void remove(Object* ob) {
			garbageObjects_.push_back(ob);
		}

		// Removes the player from memory in a safe way.
		// Not removed immediately. The removal is performed 
		// in the end of every physics update.
		void remove(Player* player) {
			garbagePlayers_.push_back(player);
		}

		// Interpolated state for the human.
		State getHumanState() const {
			return humanState_;
		}

	private:
		// Updates the game time by (msDeltaTime).
		void updatePhysics(float timeStep);

		// Removes units and players in safe way. I.e. No risk of either is in current use.
		void removeGarbage();

		void unitEventHandler(Unit* unit, Unit::UnitEvent unitEvent);
		void carEventHandler(Car* car, Car::CarEvent unitEvent);

		void doAction(Unit* unit);
		void doAction(Car* unit);
		void doShotDamage(Unit* shooter, const Bullet& properties);

		void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) override;

		void BeginContact(b2Contact* contact) override;

		// Is called when the contact has get out of view or is destroyed.
		void EndContact(b2Contact* contact) override;

		bool started_; // The game is started.
		float time_; // Local game time.

		Unit* human_;

		// Fix timestep.
		float timeStep_;
		float accumulator_;

		b2World* world_;
		GameInterface& gameInterface_;
		
		// Is cleared, and called every update to physics, in a safe way.
		// All players are assumed to be on the heap and is deallocated.
		std::vector<Player*> garbagePlayers_;
		std::list<Unit*> units_; // All units except the human unit.
		
		// Same as the abowe but for objects.
		std::vector<Object*> garbageObjects_;

		float impulseThreshold_;
		State humanState_;
	};

} // Namespace zombie.

#endif // ZOMBIEENGINE_H
