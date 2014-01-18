#ifndef GAMEDATA_H
#define GAMEDATA_H

#include "buildingproperties.h"
#include "weaponproperties.h"
#include "unitproperties.h"
#include "carproperties.h"
#include "settings.h"
#include "animation.h"
#include "auxiliary.h"
#include "state.h"

#include <mw/sound.h>
#include <mw/texture.h>
#include <tinyxml2.h>

#include <functional>
#include <map>

namespace zombie {

	// Returns a random postion between the defined outer and inner circle centered in position.
	Position generatePosition(Position position, float innerRadius, float outerRadius);

	class GameData {
	public:
		GameData(std::string dataFile);

		inline int getWidth() const {
			return settings_.width_;
		}

		inline int getHeight() const {
			return settings_.height_;
		}

		void humanPlayer(std::function<void(State, UnitProperties, const Animation& animation)> func);

		void iterateUnits(std::function<void(State, UnitProperties, const Animation& animation)> func);

		void iterateCars(std::function<void(State, CarProperties, const mw::Sprite&)> func);

		void iterateBuildings(std::function<void(BuildingProperties)> func);

	private:
		// Handle to first node <zombie>.
		bool load(tinyxml2::XMLHandle xml);

		// Handle to map (node = <map>) to be loaded.
		void loadMap(std::string map);

		// Get the loaded texture, if the loaded texture not exist in memory,
		// the image (file) is loaded to memory. If the loading fails a nullptr
		// is returned.
		mw::TexturePtr getLoadedTexture(std::string file);

		Settings settings_;
		std::map<std::string, WeaponProperties> weapons_;
		std::map<std::string, UnitProperties> units_;
		std::map<std::string, CarProperties> cars_;
		std::vector<BuildingProperties> buildings_;

		std::map<std::string, mw::TexturePtr> textures_;
		std::map<std::string, mw::Sound> sounds_;
	};
	
} // Namespace zombie.

#endif // ZOMBIEENGINE_H
