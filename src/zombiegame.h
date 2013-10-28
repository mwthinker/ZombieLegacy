#ifndef ZOMBIEGAME_H
#define ZOMBIEGAME_H

#include "zombieengine.h"
#include "device.h"
#include "buildingproperties.h"
#include "weaponproperties.h"
#include "unitproperties.h"
#include "carproperties.h"
#include "settings.h"

#include <mw/texture.h>
#include <mw/sprite.h>
#include <mw/sound.h>

#include <tinyxml2.h>

#include <map>

namespace zombie {

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

		// Handle to first node <zombie>.
		bool load(tinyxml2::XMLHandle xml);
		
		// Handle to map (node = <map>) to be loaded.
		void loadMap(std::string map);

		// Get the loaded texture, if the loaded texture not exist in memory,
		// the image (file) is loaded to memory. If the loading fails a nullptr
		// is returned.
		mw::TexturePtr getLoadedTexture(std::string file);
				
		float innerSpawnRadius_;
		float outerSpawnRadius_;
		int unitLevel_;
		int zoomlevel_;

		ZombieEngine engine_;
		DevicePtr keyboard1_, keyboard2_;

		Settings settings_;
		std::map<std::string, WeaponProperties> weapons_;
		std::map<std::string, UnitProperties> units_;
		std::map<std::string, CarProperties> cars_;
		std::vector<BuildingProperties> buildings_;

		std::map<std::string, mw::TexturePtr> textures_;
		std::map<std::string, mw::Sound> sounds_;
	};

} // Namespace zombie_;

#endif // ZOMBIEGAME_H
