#ifndef GAMEDATAENTRY_H
#define GAMEDATAENTRY_H

#include "animation.h"
#include "box2ddef.h"

#include <mw/sound.h>
#include <mw/texture.h>
#include <mw/font.h>
#include <mw/color.h>
#include <mw/music.h>

#include <xml/dataentry.h>

#include <string>
#include <functional>

namespace zombie {

	// Takes a string as input and returns the points.
	// The string "POLYGON ((x1 y1, x2 y2, ...))" the input should be defined
	// as "POLYGON ((...))". The last point is assumed to be the same as the first, therefore
	// the last point will not be includeded in the returned vector.
	std::vector<Point> loadPolygon(std::string line);

	// Takes a string as input and returns the point.
	// The string "POINT (x y)" the input should be defined
	// as "POINT (...)".
	Point loadPoint(std::string line);

	class GameDataEntry : public xml::DataEntry {
	public:
		GameDataEntry(std::string fileName);

		GameDataEntry getDeepChildEntry(std::string tagNames) const;

		GameDataEntry getChildEntry(std::string tagName) const;

		GameDataEntry getSibling(std::string siblingName) const;
		
		mw::Font getFont(int size) const;
		mw::Sound getSound() const;
		mw::Music getMusic() const;
		Animation getAnimation() const;
		mw::Sprite getSprite() const;
		mw::Texture getTexture() const;
		mw::Color getColor() const;
		Position getPosition() const;

	private:
		class GameData {
		public:
			mw::Sprite extractSprite(GameDataEntry entry) const;

			Animation extractAnimation(GameDataEntry entry) const;

			mw::Sound extractSound(GameDataEntry entry) const;

			mw::Music extractMusic(GameDataEntry entry) const;

			void loadFrame(GameDataEntry, Animation& animation) const;

			mw::Font loadFont(std::string file, unsigned int fontSize) const;
			mw::Sound loadSound(std::string file) const;
			mw::Texture loadTexture(std::string file) const;
			mw::Music loadMusic(std::string file) const;

			mutable std::map<std::string, mw::Texture> textures_;
			mutable std::map<std::string, mw::Sound> sounds_;
			mutable std::map<std::string, mw::Font> fonts_;
			mutable std::map<std::string, mw::Music> musics_;
		};

		GameDataEntry(const GameDataEntry& g, xml::DataEntry e);
		
		std::shared_ptr<GameData> gameData_;
	};

} // Namespace zombie.

#endif // GAMEDATAENTRY_H
