#ifndef ZOMBIEGAME_H
#define ZOMBIEGAME_H

#include "inputkeyboard.h"
//#include "inputjoystick.h"

#include "quadtree.h"
#include "input.h"
#include "map.h"
#include "bullet.h"

#include "typedefs.h"

#include <Box2D/Box2D.h>
#include <SDL.h>

#include <memory>
#include <vector>
#include <string>

namespace zombie {

	// Forward declaration.
	class TaskManager;
	class Car;

	class ZombieGame {
	public:
		ZombieGame(int width = 500, int height = 500);
		~ZombieGame();

		// Starts the game. The connection need to be active else nothing happens.
		void startGame();

		// Draws the graphic and (deltaTime) should be the time past 
		// from the previous call to this funtion.
		void update(float deltaTime);

		// Makes the game reacting on the evennt (windowEvent).
		void eventUpdate(const SDL_Event& windowEvent);

		void zoom(double scale);

		void updateSize(int width, int height);

	protected:
		// Updates the game time by (msDeltaTime).
		void updatePhysics(float timeStep);

		void reshapeWindowsOpenGL(int width, int height) {		
		}

		// Add a human player (unitPtr) to the game.
		void addHuman(HumanPlayerPtr human, Unit* unit);

		// Add a new ai (unitPtr) to the game.
		void addNewAi(Unit* unit);

		//void updateWindowSize(int width, int height);

		// sets the most south west building in "origo"
		void normalizeBuildings();

		// Inits the game.
		void initGame();

		// Returns a vector of all units visible by the unit (unit).
		std::vector<Unit*> calculateUnitsInView(Unit* unit);	

		void doShotDamage(Unit* shooter, const Bullet& properties);
		bool isVisible(Unit* unitToBeSeen, Unit* unitThatSees) const;

		void spawnAndCleanUpUnits();	// Spawns new zombies

		bool started_; // The game time is started.	
		float time_; // Local game time.

		typedef std::pair<HumanPlayerPtr,Unit*> PairHumanUnit;
		typedef std::pair<AiPlayerPtr,Unit*> PairAiUnit;
		typedef std::tuple<PlayerPtr,Unit*, Car*> PairPlayerUnit;

		std::vector<PairHumanUnit> humanPlayers_;
		std::vector<PairAiUnit> aiPlayers_;
		std::vector<PairPlayerUnit> players_; // All units.

		Quadtree<BuildingPtr> buildings_; // All buildings.
		TaskManager* taskManager_;

		int indexAiPlayer_;
		double timeToUpdateView_;
		double timeToUpdateSpawn_;
		double timeSinceSpawn_;

		int unitLevel_; // Specifies the wanted number of zombies on the map
		double scale_;
		float innerSpawnRadius_;
		float outerSpawnRadius_;
		Map map_;

		Position viewPosition_;

		float timeStep_;
		float accumulator_;

		b2World* world_;
		Bullet lastBullet_;
	};

} // namespace zombie

#endif // ZOMBIEGAME_H