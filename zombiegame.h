#ifndef ZOMBIEGAME_H
#define ZOMBIEGAME_H

#include "inputkeyboard.h"
//#include "inputjoystick.h"

#include "input.h"
#include "map.h"
#include "bullet.h"
#include "graphicobject.h"

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

	class ZombieGame : public b2ContactListener {
	public:
		ZombieGame(int width = 500, int height = 500);
		~ZombieGame();

		// Starts the game.
		void startGame();

		// Draws the graphic and (deltaTime) should be the time past 
		// from the previous call to this funtion.
		void update(float deltaTime);

		// Makes the game reacting on an event (windowEvent).
		void eventUpdate(const SDL_Event& windowEvent);

		void zoom(double scale);

		void updateSize(int width, int height);

	protected:
		// Updates the game time by (msDeltaTime).
		void updatePhysics(float timeStep);

		// Creates a unit and add &doShotDamage to receive bullets fired.
		Unit* createUnit(float x, float y, float angle, const Weapon& weapon, bool infected);

		// Add a human player (unitPtr) to the game.
		void addHuman(HumanPlayerPtr human, Unit* unit);

		// Add a new ai (unitPtr) to the game.
		void addNewAi(Unit* unit);

		// Inits the game.
		void initGame();

		// Returns a vector of all units visible by the unit (unit).
		std::vector<Unit*> calculateUnitsInView(Unit* unit);

		void doAction(Unit* unit);
		void doShotDamage(Unit* shooter, const Bullet& properties);

		void BeginContact (b2Contact* contact) override;
		void EndContact (b2Contact* contact) override;
		void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) override;

		void spawnAndCleanUpUnits(); // Spawns new zombies.

		bool started_; // The game is started.
		float time_; // Local game time.
		
		typedef std::tuple<PlayerPtr,MovingObject*,GraphicObject*> TuplePlayerUnitGraphic;
		
		std::list<TuplePlayerUnitGraphic> players_; // All units.
		
		TaskManager* taskManager_;

		int unitLevel_; // Specifies the wanted number of zombies on the map.
		double scale_;
		float innerSpawnRadius_;
		float outerSpawnRadius_;
		Map map_;

		// The view is centered in.
		Position viewPosition_;

		// Fix timestep.
		float timeStep_;
		float accumulator_;

		b2World* world_;

		mw::Signal<const SDL_Event&> sdlEventSignal_;
	};

} // namespace zombie

#endif // ZOMBIEGAME_H
