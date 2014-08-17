#ifndef GAMEDATAENTRY_H
#define GAMEDATAENTRY_H

#include "animation.h"
#include "box2ddef.h"

#include <mw/sound.h>
#include <mw/texture.h>
#include <mw/font.h>

#include <tinyxml2.h>

#include <string>
#include <functional>

namespace zombie {

	class GameData;

	class GameDataEntry {
	public:
		friend class GameData;

		GameDataEntry(const GameDataEntry& GameDataEntry);

		bool isAttributeEqual(std::string name, std::string value) const;
		bool isAttributeEqual(std::string tagName, std::string name, std::string value) const;

		float getFloat(std::string tagName) const;
		int getInt(std::string tagName) const;
		mw::Sound getSound(std::string tagName) const;
		Animation getAnimation(std::string tagName) const;
		mw::Sprite getSprite(std::string tagName) const;
		mw::Texture getTexture(std::string tagName) const;
		std::string getString(std::string tagName) const;
		mw::Color getColor(std::string tagName) const;

		GameDataEntry getChildEntry(std::string tagName) const;

		void iterateChilds(std::string tagName, const std::function<bool(GameDataEntry)>& next) const;

	private:
		GameDataEntry(const GameData& gameData, tinyxml2::XMLConstHandle tag);

		tinyxml2::XMLConstHandle tag_;
		const GameData& gameData_;
	};

} // Namespace zombie.

#endif // GAMEDATAENTRY_H
