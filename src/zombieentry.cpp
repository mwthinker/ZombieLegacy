#include "zombieentry.h"

#include <stdexcept>
#include <sstream>
#include <iostream>

namespace xml {

	template <>
	zombie::Point extract(tinyxml2::XMLHandle handle) {
		const tinyxml2::XMLElement* element = handle.ToElement();
		if (element == nullptr) {
			throw std::runtime_error("Missing element!");
		}
		const char* str = element->GetText();

		if (str == nullptr) {
			throw std::runtime_error("Missing text!");
		}

		std::stringstream stream(str);

		char chr = 0;;
		zombie::Point point;
		stream >> chr;
		if (chr != '(') {
			throw std::runtime_error("Missing '('!");
		}
		if (!(stream >> point.x)) {
			throw std::runtime_error("x coord invalid");
		}
		if (!(stream >> point.y)) {
			throw std::runtime_error("y coord invalid");
		}
		chr = 0;
		stream >> chr;
		if (chr != ')') {
			throw std::runtime_error("Missing ')'!");
		}

		return point;
	}

	template <>
	mw::Color extract(tinyxml2::XMLHandle handle) {
		tinyxml2::XMLElement* element = handle.ToElement();
		if (element == nullptr) {
			throw std::runtime_error("Missing element!");
		}
		const char* str = element->GetText();

		if (str == nullptr) {
			throw std::runtime_error("Missing text!");
		}

		std::stringstream stream(str);
		char chr = 0;;
		mw::Color color;
		stream >> chr;
		if (chr != '(') {
			throw std::runtime_error("Missing '('!");
		}
		if (!(stream >> color.red_)) {
			throw std::runtime_error("Red value invalid");
		}
		if (!(stream >> color.green_)) {
			throw std::runtime_error("Green value invalid");
		}
		if (!(stream >> color.blue_)) {
			throw std::runtime_error("Blue value invalid");
		}
		// Assume that everything is correct.
		stream >> color.alpha_;
		return color;
	}

} // Namespace xml.

namespace zombie {

	Point loadPoint(std::string line) {
		std::stringstream stream(line);

		std::string word;
		stream >> word;
		assert(word == "POINT");

		char chr;
		stream >> chr;
		assert(chr == '(');
		Point point;
		stream >> point.x;
		stream >> point.y;
		return point;
	}

	std::vector<Point> loadPolygon(std::string line) {
		std::stringstream stream(line);
		std::string word;

		stream >> word;
		assert(word == "POLYGON");

		char paranthes;
		stream >> paranthes;
		assert(paranthes == '(');
		stream >> paranthes;
		assert(paranthes == '(');

		Point point;
		std::vector<Point> points;

		while (stream >> point.x && stream >> point.y) {
			points.push_back(point);
			char comma;
			if (!(stream >> comma)) {
				break;
			}
		}
		points.pop_back();
		return points;
	}

	ZombieEntry::ZombieEntry(std::string fileName) : xml::DataEntry(fileName), gameData_(std::make_shared<GameData>()) {
	}

	ZombieEntry ZombieEntry::getDeepChildEntry(std::string tagNames) const {
		return ZombieEntry(*this, xml::DataEntry::getDeepChildEntry(tagNames));
	}

	ZombieEntry ZombieEntry::getChildEntry(std::string tagName) const {
		return ZombieEntry(*this, xml::DataEntry::getChildEntry(tagName));
	}	

	ZombieEntry ZombieEntry::getSibling(std::string siblingName) const {
		return ZombieEntry(*this, DataEntry::getSibling(siblingName));
	}

	ZombieEntry ZombieEntry::getParent() const {
		return ZombieEntry(*this, DataEntry::getParent());
	}

	ZombieEntry::ZombieEntry(const ZombieEntry& g, xml::DataEntry e) : xml::DataEntry(e) {
		gameData_ = g.gameData_;
	}

	mw::Font ZombieEntry::getFont(int size) const {
		return gameData_->loadFont(getString(), size);
	}

	mw::Sound ZombieEntry::getSound() const {
		return gameData_->extractSound(*this);
	}

	mw::Music ZombieEntry::getMusic() const {
		return gameData_->extractMusic(*this);
	}

	Animation ZombieEntry::getAnimation() const {
		return gameData_->extractAnimation(*this);
	}

	mw::Sprite ZombieEntry::getSprite() const {
		return gameData_->extractSprite(*this);
	}	

	mw::Color ZombieEntry::getColor() const {
		return get<mw::Color>();
	}

	Position ZombieEntry::getPosition() const {
		return get<Position>();
	}

	void ZombieEntry::GameData::loadFrame(ZombieEntry entry, Animation& animation, float deltaTime, float bodyWidth) const {
		mw::Sprite sprite = extractSprite(entry.getChildEntry("image"));
				
		auto timeEntry = entry.getChildEntry("time");
		if (timeEntry.hasData()) {
			deltaTime = timeEntry.getFloat();
		}
		auto widthEntry = entry.getChildEntry("bodyWidth");
		if (widthEntry.hasData()) {
			bodyWidth = widthEntry.getFloat();
		}

		animation.add(sprite, bodyWidth, deltaTime);
	}

	mw::Sprite ZombieEntry::GameData::extractSprite(ZombieEntry entry) const {
		return textureAtlas_.add(entry.getString(), 2);
	}

	Animation ZombieEntry::GameData::extractAnimation(ZombieEntry entry) const {
		Animation animation;
		float deltaTime = 1.f;
		auto tmp = entry.getChildEntry("deltaTime");
		if (tmp.hasData()) {
			deltaTime = tmp.getFloat();
		}
		tmp = entry.getChildEntry("bodyWidth");
		float bodyWidth = 1;
		if (tmp.hasData()) {
			bodyWidth = tmp.getFloat();
		}
		
		entry = entry.getChildEntry("frame");
		while (entry.hasData()) {
			loadFrame(entry, animation, deltaTime, bodyWidth);
			entry = entry.getSibling("frame");
		}
		return animation;
	}

	mw::Sound ZombieEntry::GameData::extractSound(ZombieEntry entry) const {
		return loadSound(entry.getString());
	}

	mw::Music ZombieEntry::GameData::extractMusic(ZombieEntry entry) const {
		return loadMusic(entry.getString());
	}

	mw::Font ZombieEntry::GameData::loadFont(std::string file, unsigned int fontSize) const {
		unsigned int size = fonts_.size();
		std::string key = file;
		key += fontSize;
		mw::Font& font = fonts_[key];

		// Font not found?
		if (fonts_.size() > size) {
			font = mw::Font(file, fontSize);
		}

		return font;
	}

	mw::Sound ZombieEntry::GameData::loadSound(std::string file) const {
		unsigned int size = sounds_.size();
		mw::Sound& sound = sounds_[file];

		// Sound not found?
		if (sounds_.size() > size) {
			sound = mw::Sound(file);
		}

		return sound;
	}

	mw::Music ZombieEntry::GameData::loadMusic(std::string file) const {
		unsigned int size = musics_.size();
		mw::Music& music = musics_[file];

		// Music not found?
		if (musics_.size() > size) {
			music = mw::Music(file);
		}

		return music;
	}

} // Namespace zombie.
