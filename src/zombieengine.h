#ifndef ZOMBIEENGINE_H
#define ZOMBIEENGINE_H

#include "object.h"
#include "state.h"
#include "device.h"
#include "input.h"
#include "typedefs.h"
#include "taskmanager.h"

#include <Box2D/Box2D.h>
#include <SDL.h>
#include <mw/signal.h>

#include <memory>
#include <vector>
#include <string>

namespace zombie {

	// Forward declaration.	
	class Car;
	class Unit;
	class Weapon;
	class MovingObject;
	class Player;
	class Bullet;
	class Animation;
	class GameInterface;

	// Responsible of all creation and deallocation of game objects 
	// and manage there physical and graphical representation.
	class ZombieEngine : public b2ContactListener {
	public:
		ZombieEngine(GameInterface* gameInterface);
		~ZombieEngine();

		// Starts the game.
		void start();

		// Draws the graphic and (deltaTime) should be the time past 
		// from the previous call to this funtion.
		void update(float deltaTime);

		void draw(float deltaTime);

		// Add a human player to the game.
		void setHuman(DevicePtr device, const State& state, float mass, float radius, float life, float walkingSpeed, float runningSpeed, const Weapon& weapon, const Animation& animation);

		// Add a ai player to the game.
		void addAi(const State& state, float mass, float radius, float life, float walkingSpeed, float runningSpeed, bool infected, const Weapon& weapon, const Animation& animation);

		// Add a car to the game.
		void addCar(const State& state, float mass, float life, float width, float length, const Animation& animation);

		void addBuilding(const std::vector<Position>& corners);

		void addWeapon(float x, float y, const Weapon& weapon);

		void addGrassGround(float minX, float maxX, float minY, float maxY);

		inline float getTime() const {
			return time_;
		}

	private:
		// Creates a unit and add &doShotDamage to receive bullets fired.
		Unit* createUnit(const State& state, float mass, float radius, float life, float walkingSpeed, float runningSpeed, bool infected, const Weapon& weapon);

		Car* createCar(const State& state, float mass, float life, float width, float length);

		// Updates the game time by (msDeltaTime).
		void updatePhysics(float timeStep);

		// Returns a vector of all units visible by the unit (unit).
		std::vector<Unit*> calculateUnitsInView(Unit* unit);

		void unitDoAction(Unit* unit);
		void carDoAction(Car* unit);
		void doShotDamage(Unit* shooter, const Bullet& properties);

		void BeginContact (b2Contact* contact) override;
		void EndContact (b2Contact* contact) override;
		void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) override;

		void spawnAndCleanUpUnits(); // Spawns new zombies.

		bool started_; // The game is started.
		float time_; // Local game time.
		
		Unit* human_;
		std::list<GameEntity*> entities_; // All units.
		TaskManager taskManager_;

		// Fix timestep.
		float timeStep_;
		float accumulator_;

		b2World* world_;
		GameInterface* gameInterface_;
	};

} // Namespace zombie.

#endif // ZOMBIEENGINE_H
