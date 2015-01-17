#ifndef ZOMBIEENTRY_H
#define ZOMBIEENTRY_H

#include "animation.h"
#include "box2ddef.h"

#include <mw/sound.h>
#include <mw/texture.h>
#include <mw/font.h>
#include <mw/color.h>
#include <mw/music.h>
#include <mw/textureatlas.h>

#include <xml/dataentry.h>

#include <string>
#include <functional>

namespace xml {

	// Template specialization. Point must be defined as "(12.3 55.3)".
	// x = 12.3, y = 55.3
	template <>
	zombie::Point extract(tinyxml2::XMLHandle handle);

	// Template specialization. Color must be defined as "(0.1 0.2 0.3)" or "(0.1 0.2 0.3 0.4)"
	// red = 0.1, green = 0.2, blue = 0.3, alpha = 0.4
	template <>
	mw::Color extract(tinyxml2::XMLHandle handle);

}

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
	
	class ZombieEntry : public xml::DataEntry {
	public:
		ZombieEntry(std::string fileName);

		ZombieEntry getDeepChildEntry(std::string tagNames) const;

		ZombieEntry getChildEntry(std::string tagName) const;

		ZombieEntry getSibling(std::string siblingName) const;

		ZombieEntry getParent() const;
		
		mw::Font getFont(int size) const;
		mw::Sound getSound() const;
		mw::Music getMusic() const;
		Animation getAnimation() const;
		mw::Sprite getSprite() const;
		mw::Color getColor() const;
		Position getPosition() const;

	private:
		class GameData {
		public:
			GameData() : textureAtlas_(2048, 2048, []() {
				mw::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				mw::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				mw::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				mw::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}) {
				
			}

			mw::Sprite extractSprite(ZombieEntry entry) const;

			Animation extractAnimation(ZombieEntry entry) const;

			mw::Sound extractSound(ZombieEntry entry) const;

			mw::Music extractMusic(ZombieEntry entry) const;

			void loadFrame(ZombieEntry, Animation& animation, float deltaTime, float bodyWidth) const;

			mw::Font loadFont(std::string file, unsigned int fontSize) const;
			mw::Sound loadSound(std::string file) const;
			mw::Texture loadTexture(std::string file) const;
			mw::Music loadMusic(std::string file) const;

			mutable mw::TextureAtlas textureAtlas_;
			mutable std::map<std::string, mw::Sound> sounds_;
			mutable std::map<std::string, mw::Font> fonts_;
			mutable std::map<std::string, mw::Music> musics_;
		};

		ZombieEntry(const ZombieEntry& g, xml::DataEntry e);
		
		std::shared_ptr<GameData> gameData_;
	};

} // Namespace zombie.

#endif // ZOMBIEENTRY_H
