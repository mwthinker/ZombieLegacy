#ifndef ZOMBIEGAME_H
#define ZOMBIEGAME_H

#include "zombieengine.h"
#include "device.h"

#include <SDL.h>
#include <tinyxml2.h>

#include <map>

namespace zombie {

	class UnitProperties {
	public:
		float mass_;
		float radius_;
		float life_;
		float walkingSpeed_;
		float runningSpeed_;
		float stamina_;
		bool infected_;
	};

	class WeaponProperties {
	public:
		float mass_;
		std::string animation_;
		float damage_;
		float timeBetweenShots_;
		float range_;
		int clipSize_;
		std::string shoot_sound_;
		std::string reload_sound_;
	};

	// Responsible of loading map, units and initiate all
	// game related things and to start the game engine.
	// It also handle all game events triggered by the game engine,
	// e.g. what happens when a unit dies.
	class ZombieGame {
	public:
		ZombieGame(int width, int height, tinyxml2::XMLHandle xml);
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

	private:
		void handleGameEvent(const GameEvent& gameEvent);
		void handleRemoval(bool& remove, MovingObject* mOb);

		//Handle to first node <zombie>.
		bool load(tinyxml2::XMLHandle xml);
		
		//Handle to first node <maps>.
		void loadMap(tinyxml2::XMLHandle xml);
		
		//Handle to first node <weapons>.
		void loadWeapons(tinyxml2::XMLHandle xml);
		
		//Handle to first node <settings>.
		void loadSettings(tinyxml2::XMLHandle xml);
		
		//Handle to first node <movingObjects>.
		void loadMovingUnits(tinyxml2::XMLHandle xml);
				
		float innerSpawnRadius_;
		float outerSpawnRadius_;

		ZombieEngine engine_;
		DevicePtr keyboard1_, keyboard2_;
		std::map<std::string, WeaponProperties> weapons_;
		std::map<std::string, UnitProperties> units_;
	};

} // Namespace zombie_;

#endif // ZOMBIEGAME_H
