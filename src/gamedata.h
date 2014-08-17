#ifndef GAMEDATA_H
#define GAMEDATA_H

#include "animation.h"
#include "box2ddef.h"

#include <mw/sound.h>
#include <mw/texture.h>
#include <mw/font.h>

#include <tinyxml2.h>

#include <functional>
#include <map>
#include <string>

namespace zombie {

	class WeaponItem2D;
	class DataInterface;

	class GameData {
	public:
		friend class GameDataEntry;

		GameData(std::string dataFile);
		GameDataEntry getEntry(std::string tagName) const;

		GameDataEntry getZombieEntry() const;

		void save();

		void load(DataInterface& dataInterface) const;

		int getWindowWidth() const;
        int getWindowHeight() const;
        void setWindowSize(int width, int height);
        void setWindowPosition(int x, int y);
        int getWindowXPosition() const;
        int getWindowYPosition() const;

        void setWindowMaximized(bool maximized);
        bool isWindowMaximized() const;

		float getImpulseThreshold() const;

		float getInnerSpawnRadius() const;
		float getOuterSpawnRadius() const;

		int getTimeStepMS() const;

		mw::Sound getMenuHighlitedSound() const;
		mw::Sound getMenuChoiceSound() const;		

		mw::Sprite getMenuBackgroundImage() const;

		mw::Sprite getTreeImage() const;

		mw::Sprite getWallImage() const;

		int getUnitLevel() const;

		int getUnitLimit() const;

		inline mw::Font getDefaultFont(int fontSize) {
            return loadFont(font_, fontSize);
        }

	private:
		void loadMap(DataInterface&) const;
				
		void loadMissile(tinyxml2::XMLConstHandle projectileTag, DataInterface&, std::string name, float damage, float timeBetweenShots, float range, int clipSize,
			const mw::Sprite& symbol, const Animation& moveAnimation, float size, Position grip, const mw::Sound& shoot, const mw::Sound& reload) const;

		mw::Sprite extractSprite(tinyxml2::XMLConstHandle handle) const;

		Animation extractAnimation(tinyxml2::XMLConstHandle animationTag) const;

		mw::Sound extractSound(tinyxml2::XMLConstHandle handle) const;

		void loadFrame(tinyxml2::XMLConstHandle frameTag, Animation& animation) const;

		mw::Font loadFont(std::string file, unsigned int fontSize) const;
		mw::Sound loadSound(std::string file) const;
		mw::Texture loadTexture(std::string file) const;

		std::string font_;
		
		mutable std::map<std::string, mw::Texture> textures_;
		mutable std::map<std::string, mw::Sound> sounds_;
		mutable std::map<std::string, mw::Font> fonts_;
		
		tinyxml2::XMLDocument xmlDoc_;
		tinyxml2::XMLDocument xmlMap_;
		std::string dataFile_;
	};

} // Namespace zombie.

#endif // ZOMBIEENGINE_H
