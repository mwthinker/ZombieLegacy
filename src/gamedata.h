#ifndef GAMEDATA_H
#define GAMEDATA_H

#include "weaponproperties.h"
#include "unitproperties.h"
#include "settings.h"
#include "animation.h"
#include "auxiliary.h"
#include "state.h"
#include "terrain2d.h"

#include <mw/sound.h>
#include <mw/texture.h>
#include <tinyxml2.h>

#include <functional>
#include <map>

namespace zombie {

	class Building2D;
	class Car2D;

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

		inline const std::map<std::string, Car2D*>& getCars() {
			return cars_;
		}

		void iterateBuildings(std::function<void(Building2D*)> func);

		inline float getImpulseThreshold() const {
			return settings_.impulseThreshold_;
		}

		inline int getTimeStemMS() const {
			return settings_.timeStepMS_;
		}

		inline Terrain2D getTerrain2D() const {
			return terrain2d_;
		}

	private:
		// Handle to first node <zombie>.
		bool load(tinyxml2::XMLHandle xml);

		// Handle to map (node = <map>) to be loaded.
		void loadMap(std::string map);

		void loadCars(tinyxml2::XMLHandle movingUnitsTag);

		// Get the loaded texture, if the loaded texture not exist in memory,
		// the image (file) is loaded to memory. If the loading fails a nullptr
		// is returned.
		mw::TexturePtr loadTexture(std::string file);

		Settings settings_;
		std::map<std::string, WeaponProperties> weapons_;
		std::map<std::string, UnitProperties> units_;
		std::map<std::string, Car2D*> cars_;
		std::vector<Building2D*> buildings_;

		std::map<std::string, mw::TexturePtr> textures_;
		std::map<std::string, mw::Sound> sounds_;

		Terrain2D terrain2d_;
	};
	
} // Namespace zombie.

#endif // ZOMBIEENGINE_H
