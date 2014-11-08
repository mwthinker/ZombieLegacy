#include "gamedataentry.h"

#include <stdexcept>
#include <sstream>
#include <iostream>

// Template specialization. Point must be defined as "(12.3 55.3)".
// x = 12.3, y = 55.3
template <>
zombie::Point xml::extract(tinyxml2::XMLHandle handle) {
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

// Template specialization. Color must be defined as "(0.1 0.2 0.3)" or "(0.1 0.2 0.3 0.4)"
// red = 0.1, green = 0.2, blue = 0.3, alpha = 0.4
template <>
mw::Color xml::extract(tinyxml2::XMLHandle handle) {
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

	GameDataEntry::GameDataEntry(std::string fileName) : xml::DataEntry(fileName), gameData_(std::make_shared<GameData>()) {
	}

	GameDataEntry GameDataEntry::getDeepChildEntry(std::string tagNames) const {
		return GameDataEntry(*this, xml::DataEntry::getDeepChildEntry(tagNames));
	}

	GameDataEntry GameDataEntry::getChildEntry(std::string tagName) const {
		return GameDataEntry(*this, xml::DataEntry::getChildEntry(tagName));
	}	

	GameDataEntry GameDataEntry::getSibling(std::string siblingName) const {
		return GameDataEntry(*this, DataEntry::getSibling(siblingName));
	}

	GameDataEntry::GameDataEntry(const GameDataEntry& g, xml::DataEntry e) : xml::DataEntry(e) {
		gameData_ = g.gameData_;
	}

	mw::Font GameDataEntry::getFont(int size) const {
		return gameData_->loadFont(getString(), size);
	}

	mw::Sound GameDataEntry::getSound() const {
		return gameData_->extractSound(*this);
	}

	mw::Music GameDataEntry::getMusic() const {
		return gameData_->extractMusic(*this);
	}

	Animation GameDataEntry::getAnimation() const {
		return gameData_->extractAnimation(*this);
	}

	mw::Sprite GameDataEntry::getSprite() const {
		return gameData_->extractSprite(*this);
	}

	mw::Texture GameDataEntry::getTexture() const {
		return gameData_->loadTexture(getString());
	}

	mw::Color GameDataEntry::getColor() const {
		return get<mw::Color>();
	}

	Position GameDataEntry::getPosition() const {
		return get<Position>();
	}

	void GameDataEntry::GameData::loadFrame(GameDataEntry entry, Animation& animation) const {
		mw::Sprite sprite = extractSprite(entry.getChildEntry("image"));

		float time = 1;
		auto timeEntry = entry.getChildEntry("time");
		if (timeEntry.hasData()) {
			time = timeEntry.getFloat();
		}

		float bodyWidth = entry.getChildEntry("bodyWidth").getFloat();

		animation.add(sprite, bodyWidth, time);
	}

	mw::Sprite GameDataEntry::GameData::extractSprite(GameDataEntry entry) const {
		float x = entry.getFloatAttribute("x");
		float y = entry.getFloatAttribute("y");
		float h = entry.getFloatAttribute("h");
		float w = entry.getFloatAttribute("w");

		mw::Texture texture = loadTexture(entry.getString());

		if (h < 1) {
			h = (float) texture.getHeight();
		}

		if (w < 1) {
			w = (float) texture.getWidth();
		}

		return mw::Sprite(texture, x, y, w, h);
	}

	Animation GameDataEntry::GameData::extractAnimation(GameDataEntry entry) const {
		Animation animation;
		entry = entry.getChildEntry("frame");
		while (entry.hasData()) {
			loadFrame(entry, animation);
			entry = entry.getSibling("frame");
		}
		return animation;
	}

	mw::Sound GameDataEntry::GameData::extractSound(GameDataEntry entry) const {
		return loadSound(entry.getString());
	}

	mw::Music GameDataEntry::GameData::extractMusic(GameDataEntry entry) const {
		return loadMusic(entry.getString());
	}

	mw::Font GameDataEntry::GameData::loadFont(std::string file, unsigned int fontSize) const {
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

	mw::Texture GameDataEntry::GameData::loadTexture(std::string file) const {
		unsigned int size = textures_.size();
		mw::Texture& texture = textures_[file];

		// Image not found?
		if (textures_.size() > size) {
			texture = mw::Texture(file);

			// Image not valid?
			if (!texture.isValid()) {
				std::cerr << std::endl << file << " failed to load!";
			}
		}

		return texture;
	}

	mw::Sound GameDataEntry::GameData::loadSound(std::string file) const {
		unsigned int size = sounds_.size();
		mw::Sound& sound = sounds_[file];

		// Sound not found?
		if (sounds_.size() > size) {
			sound = mw::Sound(file);
		}

		return sound;
	}

	mw::Music GameDataEntry::GameData::loadMusic(std::string file) const {
		unsigned int size = musics_.size();
		mw::Music& music = musics_[file];

		// Music not found?
		if (musics_.size() > size) {
			music = mw::Music(file);
		}

		return music;
	}

} // Namespace zombie.
