#ifndef GAMEDATA_H
#define GAMEDATA_H

#include "animation.h"
#include "explosion.h"
#include "unitproperties.h"
#include "weaponproperties.h"
#include "missileproperties.h"
#include "buildingproperties.h"

#include <mw/sound.h>
#include <mw/sprite.h>
#include <mw/color.h>
#include <mw/font.h>
#include <mw/music.h>
#include <mw/textureatlas.h>

#include <nlohmann/json.hpp>

#include <map>
#include <vector>

namespace zombie {

	class GameData {
	public:
		static GameData& getInstance() {
			static GameData instance;
			return instance;
		}

		GameData(const GameData&) = delete;
		GameData& operator=(const GameData&) = delete;

		void save();

		mw::Font loadFont(std::string file, unsigned int fontSize);
		mw::Sound loadSound(std::string file);
		mw::Music loadMusic(std::string file);
		mw::Sprite loadSprite(std::string file);

		mw::Font getDefaultFont(int size);

		void bindTextureFromAtlas() const;

		int getWindowPositionX();
		int getWindowPositionY();

		void setWindowPositionX(int x);
		void setWindowPositionY(int y);

		int getWindowWidth();
		int getWindowHeight();

		void setWindowWidth(int width);
		void setWindowHeight(int height);

		bool isWindowResizable();
		void setWindowResizable(bool resizeable);

		int getWindowMinWidth();
		int getWindowMinHeight();
		std::string getWindowIcon();
		bool isWindowBordered();
		void setWindowBordered(bool border);

		bool isWindowMaximized();
		void setWindowMaximized(bool maximized);

		bool isWindowVsync();
		void setWindowVsync(bool activate);

		bool isMusicOn();
		void setMusicOn(bool activate);

		float getMusicVolume();
		void setMusicVolume(float volume);

		mw::Music getMusicTrack();

		mw::Sprite getTreeImage();

		mw::Sprite getBuildingWallImage();

		float getSettingsImpulseThreshold();
		float getSettingsTimeStep();

		float getSettingsInnerSpawnRadius();
		float getSettingsOuterSpawnRadius();

		int getSettingsUnitLevel();
		int getSettingsUnitLimit();

		std::string getSettingsMap();

		mw::Sound getMenuSoundChoice();
		mw::Sound getMenuSoundHighlited();
		mw::Sprite getMenuBackgroundImage();

		mw::Sprite getWaterSeeFloorImage();

		mw::Sprite getRoadIntersection();
		mw::Sprite getRoadStraight0();
		mw::Sprite getRoadStraight90();
		mw::Sprite getRoadTurn0();
		mw::Sprite getRoadTurn90();
		mw::Sprite getRoadTurn180();
		mw::Sprite getRoadTurn270();
		mw::Sprite getRoadTurnIntersection0();
		mw::Sprite getRoadTurnIntersection90();
		mw::Sprite getRoadTurnIntersection180();
		mw::Sprite getRoadTurntersection270();

		ExplosionProperties getExplosionProperties();

		UnitProperties getHumanProperties();
		UnitProperties getZombieProperties();

		MissileProperties getMissileProperties();

		MapProperties loadMapProperties();

	private:
		GameData();
		void loadAllWeaponProperties();
		void loadAllMissileProperties();
		void loadAllUnitProperties();

		Animation loadAnimation(const nlohmann::json& animationTag);

		UnitProperties loadUnitProperties(const nlohmann::json& unitTag);
		
		WeaponProperties loadWeaponProperties(const nlohmann::json& unitTag);
		WeaponProperties loadWeaponProperties(std::string weaponName);
		
		MissileProperties loadMissileProperties(const nlohmann::json& unitTag);
		MissileProperties loadMissileProperties(std::string weaponName);

		static const std::string SETTINGS_PATH;
		static const std::string MAPS_PATH;
		static const std::string MISSILES_PATH;
		static const std::string UNITS_PATH;
		static const std::string CARS_PATH;
		static const std::string WEAPONS_PATH;
		
		mw::TextureAtlas textureAtlas_;
		std::map<std::string, mw::Sound> sounds_;
		std::map<std::string, mw::Font> fonts_;
		std::map<std::string, mw::Music> musics_;
		std::map<std::string, WeaponProperties> weaponPropertiesMap_;
		std::map<std::string, MissileProperties> missilePropertiesMap_;
		std::map<std::string, UnitProperties> unitPropertiesMap_;

		nlohmann::json settings_;
		nlohmann::json rootMap_;
	};

} // Namespace zombie.

#endif // GAMEDATA_H
