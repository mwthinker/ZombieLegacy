#ifndef GAMEDATA_H
#define GAMEDATA_H

#include "settings.h"
#include "animation.h"
#include "state.h"
#include "terrain2d.h"
#include "weaponitem2d.h"

#include <mw/sound.h>
#include <mw/texture.h>
#include <mw/font.h>
#include <tinyxml2.h>

#include <functional>
#include <map>

namespace zombie {

	class Building2D;
	class Car2D;
	class Unit2D;
	class WeaponItem2D;

	class GameData {
	public:
		GameData(std::string dataFile);

		inline int getWidth() const {
			return settings_.width_;
		}

		inline int getHeight() const {
			return settings_.height_;
		}

		inline const std::map<std::string, Unit2D*>& getUnits() {
			return units_;
		}

		inline const std::map<std::string, Car2D*>& getCars() {
			return cars_;
		}

		const std::vector<Building2D*>& getBuildings() {
			return buildings_;
		}

		inline float getImpulseThreshold() const {
			return settings_.impulseThreshold_;
		}

		inline int getTimeStemMS() const {
			return settings_.timeStepMS_;
		}

		inline Terrain2D getTerrain2D() const {
			return terrain2d_;
		}

		inline int getUnitLevel() const {
			return settings_.unitLevel_;
		}

		inline mw::Font getDefaultFont(int size) {
			return loadFont(settings_.defaultFont_, size);
		}

		inline const std::vector<Position>& getSpawningPoints() const {
			return spawningPoints_;		
		}

	private:
		// Handle to first node <zombie>.
		bool load(tinyxml2::XMLHandle xml);

		// Handle to map (node = <map>) to be loaded.
		void loadMap(std::string map);

		void loadUnits(tinyxml2::XMLHandle movingUnitsTag);

		void loadCars(tinyxml2::XMLHandle movingCarsTag);

		void loadWeapons(tinyxml2::XMLHandle weaponsTag);

		Animation loadAnimation(tinyxml2::XMLHandle animationTag);

		void loadFrame(tinyxml2::XMLHandle frameTag, Animation& animation);

		// Get the loaded texture, if the loaded texture not exist in memory,
		// the image (file) is loaded to memory. If the loading fails a nullptr
		// is returned.
		mw::Texture loadTexture(std::string file);
		mw::Font loadFont(std::string file, unsigned int size);

		Settings settings_;
		std::map<std::string, WeaponPtr> weapons_;
		std::map<std::string, Unit2D*> units_;
		std::map<std::string, Car2D*> cars_;
		std::vector<Building2D*> buildings_;

		std::vector<Position> spawningPoints_;
		std::map<std::string, mw::Texture> textures_;
		std::map<std::string, mw::Sound> sounds_;
		std::map<std::string, mw::Font> fonts_;

		Terrain2D terrain2d_;
	};
	
} // Namespace zombie.

#endif // ZOMBIEENGINE_H
