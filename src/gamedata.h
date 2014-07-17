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

		void save();

		int getWindowWidth() const;
        int getWindowHeight() const;
        void setWindowSize(int width, int height);
        void setWindowPosition(int x, int y);
        int getWindowXPosition() const;
        int getWindowYPosition() const;

        void setWindowMaximized(bool maximized);
        bool isWindowMaximized() const;

        bool loadCar(std::string name, const Car2D& car);
        bool loadUnit(std::string name, const Unit2D& car);

        std::vector<Building2D*> loadBuildings();

		float getImpulseThreshold() const;

		int getTimeStemMS() const;

		Terrain2D getTerrain2D() const;

		int getUnitLevel() const;

		inline mw::Font getDefaultFont(int fontSize) {
            return loadFont(font_, fontSize);
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

		mw::Font loadFont(std::string file, unsigned int fontSize);
        mw::Sound loadSound(std::string file);
		mw::Texture loadTexture(std::string file);

		std::string font_;

		std::map<std::string, WeaponPtr> weapons_;

		std::vector<Position> spawningPoints_;
		std::map<std::string, mw::Texture> textures_;
		std::map<std::string, mw::Sound> sounds_;
		std::map<std::string, mw::Font> fonts_;

		Terrain2D terrain2d_;
		tinyxml2::XMLDocument xmlDoc_;
		std::string dataFile_;
	};

} // Namespace zombie.

#endif // ZOMBIEENGINE_H
