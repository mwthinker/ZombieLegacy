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

		bool loadCar(std::string name, Car2D& car) const;
		bool loadUnit(std::string name, Unit2D& car) const;

		bool loadBuildings(std::vector<Building2D>& buildings) const;

		float getImpulseThreshold() const;

		int getTimeStemMS() const;

		Terrain2D getTerrain2D() const;

		int getUnitLevel() const;

		inline mw::Font getDefaultFont(int fontSize) {
            return loadFont(font_, fontSize);
        }

		std::vector<Position> getSpawningPoints() const;

	private:
		void loadWeapons() const;

		Animation loadAnimation(tinyxml2::XMLConstHandle animationTag) const;

		void loadFrame(tinyxml2::XMLConstHandle frameTag, Animation& animation) const;

		mw::Font loadFont(std::string file, unsigned int fontSize) const;
		mw::Sound loadSound(std::string file) const;
		mw::Texture loadTexture(std::string file) const;

		std::string font_;

		mutable std::map<std::string, WeaponPtr> weapons_;
		mutable std::map<std::string, mw::Texture> textures_;
		mutable std::map<std::string, mw::Sound> sounds_;
		mutable std::map<std::string, mw::Font> fonts_;
		
		tinyxml2::XMLDocument xmlDoc_;
		tinyxml2::XMLDocument xmlMap_;
		std::string dataFile_;
	};

} // Namespace zombie.

#endif // ZOMBIEENGINE_H
