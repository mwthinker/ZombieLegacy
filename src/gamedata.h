#ifndef GAMEDATA_H
#define GAMEDATA_H

#include "animation.h"
#include "box2ddef.h"

#include <mw/sound.h>
#include <mw/texture.h>
#include <mw/font.h>
#include <mw/music.h>

#include <tinyxml2.h>

#include <functional>
#include <map>
#include <string>

namespace zombie {

	class WeaponItem2D;

	// Takes a string as input and returns the points.
	// The string "((x1 y1, x2 y2, ...))" the input should be defined
	// as ((...). The last point is assumed to be the same as the first, therefore
	// the last point will not be returned.
	std::vector<Point> loadPolygon(std::string line);

	Point loadPoint(std::string line);

	class GameData {
	public:
		friend class GameDataEntry;

		GameData(std::string dataFile);
		~GameData();

		GameDataEntry getEntry(std::string tagName) const;

		GameDataEntry getMapEntry() const;

		int getWindowWidth() const;
        int getWindowHeight() const;
        void setWindowSize(int width, int height);
        void setWindowPosition(int x, int y);
        int getWindowXPosition() const;
        int getWindowYPosition() const;
        void setWindowMaximized(bool maximized);
        bool isWindowMaximized() const;

		inline mw::Font getDefaultFont(int fontSize) {
            return loadFont(font_, fontSize);
        }

	private:
		mw::Sprite extractSprite(tinyxml2::XMLConstHandle handle) const;

		Animation extractAnimation(tinyxml2::XMLConstHandle animationTag) const;

		mw::Sound extractSound(tinyxml2::XMLConstHandle handle) const;

		mw::Music extractMusic(tinyxml2::XMLConstHandle handle) const;

		void loadFrame(tinyxml2::XMLConstHandle frameTag, Animation& animation) const;

		mw::Font loadFont(std::string file, unsigned int fontSize) const;
		mw::Sound loadSound(std::string file) const;
		mw::Texture loadTexture(std::string file) const;
		mw::Music loadMusic(std::string file) const;

		std::string font_;
		
		mutable std::map<std::string, mw::Texture> textures_;
		mutable std::map<std::string, mw::Sound> sounds_;
		mutable std::map<std::string, mw::Font> fonts_;
		mutable std::map<std::string, mw::Music> musics_;
		
		tinyxml2::XMLDocument xmlDoc_;
		tinyxml2::XMLDocument xmlMap_;
		std::string dataFile_;
	};

} // Namespace zombie.

#endif // ZOMBIEENGINE_H
