#ifndef ZOMBIEENGINE_H
#define ZOMBIEENGINE_H

#include "object.h"
#include "device.h"
#include "input.h"
#include "typedefs.h"

#include <Box2D/Box2D.h>
#include <SDL.h>
#include <mw/signal.h>

#include <memory>
#include <vector>
#include <string>

namespace zombie {

	// Forward declaration.
	class TaskManager;
	class Car;
	class Unit;
	class Weapon;
	class MovingObject;
	class Player;
	class Bullet;

	class GameEvent {
	public:
		virtual ~GameEvent() {
		}
	};

	class UnitDie : public GameEvent {
	public:
		UnitDie(Unit* unit) : unit_(unit) {
		}

		const Unit* unit_;
	};

	// Responsible of all creation and deallocation of game objects 
	// and manage there physical and graphical representation.
	class ZombieEngine : public b2ContactListener {
	public:
		ZombieEngine(int width = 500, int height = 500);
		~ZombieEngine();

		// Starts the game.
		void start();

		// Draws the graphic and (deltaTime) should be the time past 
		// from the previous call to this funtion.
		void update(float deltaTime);

		// Makes the game reacting on an event (windowEvent).
		void eventUpdate(const SDL_Event& windowEvent);

		void zoom(double scale);

		void updateSize(int width, int height);

		// Add a human player to the game.
		void addHuman(DevicePtr device, float x, float y, float angle, float mass, float radius, float life, float walkingSpeed, float runningSpeed, const Weapon& weapon);

		// Add a ai player to the game.
		void addAi(float x, float y, float angle, float mass, float radius, float life, float walkingSpeed, float runningSpeed, bool infected, const Weapon& weapon);

		// Add a car to the game.
		void addCar(float x, float y, float angle, float mass, float life, float width, float length);

		void addBuilding(const std::vector<Position>& corners);

		void addWeapon(float x, float y, const Weapon& weapon);

		void addGrassGround(float minX, float maxX, float minY, float maxY);

		Position getMainUnitPostion();

		mw::signals::Connection addEventListener(mw::Signal<const GameEvent&>::Callback callback);
		mw::signals::Connection addRemoveListener(mw::Signal<bool&, const MovingObject*>::Callback callback);

	private:
		// Creates a unit and add &doShotDamage to receive bullets fired.
		Unit* createUnit(float x, float y, float angle, float mass, float radius, float life, float walkingSpeed, float runningSpeed, bool infected, const Weapon& weapon);

		Car* createCar(float x, float y, float angle, float mass, float life, float width, float length);

		// Updates the game time by (msDeltaTime).
		void updatePhysics(float timeStep);

		// Returns a vector of all units visible by the unit (unit).
		std::vector<Unit*> calculateUnitsInView(Unit* unit);

		void doAction(Unit* unit);
		void carDoAction(Car* unit);
		void doShotDamage(Unit* shooter, const Bullet& properties);

		void BeginContact (b2Contact* contact) override;
		void EndContact (b2Contact* contact) override;
		void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) override;

		void spawnAndCleanUpUnits(); // Spawns new zombies.

		bool started_; // The game is started.
		float time_; // Local game time.
		
		std::list<Player*> players_; // All units.
		std::list<Car*> cars_; // All cars.
		TaskManager* taskManager_;

		int unitLevel_; // Specifies the wanted number of zombies on the map.
		double scale_;

		// The view is centered in.
		Position viewPosition_;

		// Fix timestep.
		float timeStep_;
		float accumulator_;

		b2World* world_;
		WorldHash worldHash_;
		mw::Signal<const GameEvent&> gameEventListener_;
		mw::Signal<bool&, const MovingObject*> removeListener_;
	};

} // Namespace zombie.

#endif // ZOMBIEENGINE_H
