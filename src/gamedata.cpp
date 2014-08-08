#include "gamedata.h"
#include "datainterface.h"

#include <mw/color.h>
#include <mw/sound.h>
#include <mw/texture.h>
#include <mw/exception.h>

#include <tinyxml2.h>

#include <map>
#include <string>
#include <sstream>
#include <iostream>

namespace zombie {

	namespace {

		// Generic template function to extract value from xml tag.
		template <class Output>
		Output extract(tinyxml2::XMLConstHandle handle) {
			const tinyxml2::XMLElement* element = handle.ToElement();
			if (element == nullptr) {
				throw mw::Exception("Missing element!");
			}
			const char* str = element->GetText();

			if (str == nullptr) {
				throw mw::Exception("Missing text!");
			}

			std::stringstream stream(str);
			Output output;
			stream >> output;
			if (stream.fail()) {
				throw mw::Exception("Stream failed!");
			}
			return output;
		}

		// Template specialization. Point must be defined as "POINT (12.3 55.3)".
		// x = 12.3, y = 55.3
		template <>
		Point extract(tinyxml2::XMLConstHandle handle) {
			const tinyxml2::XMLElement* element = handle.ToElement();
			if (element == nullptr) {
				throw mw::Exception("Missing element!");
			}
			const char* str = element->GetText();

			if (str == nullptr) {
				throw mw::Exception("Missing text!");
			}

			std::stringstream stream(str);
			std::string word;
			stream >> word;
			if (word != "POINT") {
				throw mw::Exception("Invalid 'POINT'!");
			}

			char chr = 0;;
			Point point;
			stream >> chr;
			if (chr != '(') {
				throw mw::Exception("Missing '('!");
			}
			if (!(stream >> point.x)) {
				throw mw::Exception("x coord invalid");
			}
			if (!(stream >> point.y)) {
				throw mw::Exception("y coord invalid");
			}
			chr = 0;
			stream >> chr;
			if (chr != ')') {
				throw mw::Exception("Missing ')'!");
			}

			return point;
		}

		// Template specialization. Color must be defined as "(0.1 0.2 0.3)" or "(0.1 0.2 0.3 0.4)"
		// red = 0.1, green = 0.2, blue = 0.3, alpha = 0.4
		template <>
		mw::Color extract(tinyxml2::XMLConstHandle handle) {
			const tinyxml2::XMLElement* element = handle.ToElement();
			if (element == nullptr) {
				throw mw::Exception("Missing element!");
			}
			const char* str = element->GetText();

			if (str == nullptr) {
				throw mw::Exception("Missing text!");
			}

			std::stringstream stream(str);
			char chr = 0;;
			mw::Color color;
			stream >> chr;
			if (chr != '(') {
				throw mw::Exception("Missing '('!");
			}
			if (!(stream >> color.red_)) {
				throw mw::Exception("Red value invalid");
			}
			if (!(stream >> color.green_)) {
				throw mw::Exception("Green value invalid");
			}
			if (!(stream >> color.blue_)) {
				throw mw::Exception("Blue value invalid");
			}
			// Assume that everything is correct.
			stream >> color.alpha_;
			return color;
		}

		// Specialization for string type. In order to insure that the whole text is 
		// returned as a string.
		template <>
		std::string extract(tinyxml2::XMLConstHandle handle) {
			const tinyxml2::XMLElement* element = handle.ToElement();
			if (element == nullptr) {
				throw mw::Exception("Missing element!");
			}
			const char* str = element->GetText();

			if (str == nullptr) {
				throw mw::Exception("Missing text!");
			}

			return std::string(str);
		}

		// Returns the value defined in the input string. 
		// E.g. input = "zombieGame interface font", returns the 
		// value defined in the tag inside <zombieGame><interface><font>value</font></interface></zombieGame>
		template <class Output>
		Output getValueFromTag(const tinyxml2::XMLDocument& xmlDoc, std::string input) {
			std::stringstream stream(input);
			std::string tag;
			tinyxml2::XMLConstHandle handleXml(xmlDoc);
			while (stream >> tag) {
				handleXml = handleXml.FirstChildElement(tag.c_str());
			}
			return extract<Output>(handleXml);
		}

		// Takes a string as input and returns the points.
		// The string "POLYGON ((x1 y1, x2 y2, ...))" the input should be defined
		// as ((...). The last point is assumed to be the same as the first, therefore
		// the last point will not be returned.
		std::vector<Point> loadPolygon(std::string line) {
			int index = line.find("((");
			int index2 = line.rfind("))");

			// New line is the string between "((" and "))".
			line = line.substr(index + 2, index2 - index - 2);

			// Replace all ',' (comma) with whitespace.
			for (char& chr : line) {
				if (chr == ',') {
					chr = ' ';
				}
			}

			Point point;
			std::stringstream stream(line);
			std::vector<Point> points;
			while (stream >> point.x && stream >> point.y) {
				points.push_back(point);
			}
			points.pop_back();
			return points;
		}

		// Saves the value in the tag defined by handle.
		template <class Value>
		void insert(const Value& value, tinyxml2::XMLHandle handle) {
			tinyxml2::XMLElement* element = handle.ToElement();
			if (element == nullptr) {
				throw mw::Exception("Missing element!");
			}

			std::stringstream stream;
			stream << value;
			if (stream.fail()) {
				throw mw::Exception("Stream failed!");
			}

			element->SetText(stream.str().c_str());
		}

	}

	GameData::GameData(std::string dataFile) : dataFile_(dataFile) {
		// Load XML file.
		xmlDoc_.LoadFile(dataFile.c_str());
		if (xmlDoc_.Error()) {
			// Failed!
			xmlDoc_.PrintError();
		} else {
			try {
				font_ = zombie::getValueFromTag<std::string>(xmlDoc_, "zombieGame interface font");

				// Load map.
				xmlMap_.LoadFile(zombie::getValueFromTag<std::string>(xmlDoc_, "zombieGame settings map").c_str());
				if (xmlMap_.Error()) {
					xmlDoc_.PrintError();
				}
			} catch (mw::Exception& e) {
				std::cerr << e.what();
			}
		}
	}

	void GameData::save() {
		xmlDoc_.SaveFile(dataFile_.c_str());
	}

	void GameData::load(DataInterface& dataInterface) const {
		loadWeapon(dataInterface);
		loadHuman(dataInterface);
		loadCar(dataInterface);
		loadZombie(dataInterface);
		loadMap(dataInterface);
		loadExplosion(dataInterface);
		loadFog(dataInterface);
	}

	mw::Sound GameData::getMenuHighlitedSound() const {
		tinyxml2::XMLConstHandle soundTag = tinyxml2::XMLConstHandle(xmlDoc_.FirstChildElement("zombieGame")).FirstChildElement("menu");
		std::string name = zombie::extract<std::string>(soundTag.FirstChildElement("soundHighlited"));
		return loadSound(name);
	}

	mw::Sound GameData::getMenuChoiceSound() const {
		tinyxml2::XMLConstHandle soundTag = tinyxml2::XMLConstHandle(xmlDoc_.FirstChildElement("zombieGame")).FirstChildElement("menu");
		std::string name = zombie::extract<std::string>(soundTag.FirstChildElement("soundChoice"));
		return loadSound(name);
	}

	void GameData::loadCar(DataInterface& dataInterface) const {
		tinyxml2::XMLConstHandle carTag = tinyxml2::XMLConstHandle(xmlDoc_.FirstChildElement("zombieGame")).FirstChildElement("car");
		Animation animation = extractAnimation(carTag.FirstChildElement("moveAnimation"));
		float mass = zombie::extract<float>(carTag.FirstChildElement("mass"));
		float width = zombie::extract<float>(carTag.FirstChildElement("width"));
		float length = zombie::extract<float>(carTag.FirstChildElement("length"));
		float life = zombie::extract<float>(carTag.FirstChildElement("life"));

		dataInterface.loadCar(mass, width, length, life, animation);
	}

	void GameData::loadHuman(DataInterface& dataInterface) const {
		tinyxml2::XMLConstHandle humanTag = tinyxml2::XMLConstHandle(xmlDoc_.FirstChildElement("zombieGame")).FirstChildElement("human");
		float mass = zombie::extract<float>(humanTag.FirstChildElement("mass"));
		float radius = zombie::extract<float>(humanTag.FirstChildElement("radius"));
		float life = zombie::extract<float>(humanTag.FirstChildElement("life"));
		float walkingSpeed = zombie::extract<float>(humanTag.FirstChildElement("walkingSpeed"));
		float runningSpeed = zombie::extract<float>(humanTag.FirstChildElement("runningSpeed"));
		float stamina = zombie::extract<float>(humanTag.FirstChildElement("stamina"));
		Animation moveA = extractAnimation(humanTag.FirstChildElement("moveAnimation"));
		Position grip;
		grip.x = zombie::extract<float>(humanTag.FirstChildElement("moveImageGripX"));
		grip.y = zombie::extract<float>(humanTag.FirstChildElement("moveImageGripY"));
		Animation injuredA = extractAnimation(humanTag.FirstChildElement("injuredAnimation"));
		Animation dieA = extractAnimation(humanTag.FirstChildElement("dieAnimation"));
		mw::Sound dieS = loadSound(zombie::extract<std::string>(humanTag.FirstChildElement("dieSound")));
		mw::Sound hitS = loadSound(zombie::extract<std::string>(humanTag.FirstChildElement("hitSound")));
		std::string weaponName = zombie::extract<std::string>(humanTag.FirstChildElement("weapon"));

		dataInterface.loadHuman(mass, radius, life, walkingSpeed, runningSpeed, stamina, moveA, grip, injuredA, dieA, dieS, hitS, weaponName);
	}

	void GameData::loadExplosion(DataInterface& dataInterface) const {
		tinyxml2::XMLConstHandle explosionTag = tinyxml2::XMLConstHandle(xmlDoc_.FirstChildElement("zombieGame")).FirstChildElement("explosion");
		mw::Texture particle = loadTexture(zombie::extract<std::string>(explosionTag.FirstChildElement("particleImage")));
		mw::Sprite shockwave = extractSprite(explosionTag.FirstChildElement("shockwaveImage"));
		mw::Sprite emitter = extractSprite(explosionTag.FirstChildElement("emitterImage"));
		mw::Sound sound = loadSound(zombie::extract<std::string>(explosionTag.FirstChildElement("shockwaveImage")));
		dataInterface.loadExplosion(particle, shockwave, emitter, sound);
	}

	void GameData::loadFog(DataInterface& dataInterface) const {
		tinyxml2::XMLConstHandle zombieGameTag = tinyxml2::XMLConstHandle(xmlDoc_.FirstChildElement("zombieGame")).FirstChildElement("fog");
		mw::Texture fog = loadTexture(zombie::extract<std::string>(zombieGameTag.FirstChildElement("image")));
		float radius = zombie::extract<float>(zombieGameTag.FirstChildElement("radius"));
		mw::Color color = zombie::extract<mw::Color>(zombieGameTag.FirstChildElement("color"));
		dataInterface.loadFog(fog, radius, color);
	}

	void GameData::loadZombie(DataInterface& dataInterface) const {
		tinyxml2::XMLConstHandle zombieTag = tinyxml2::XMLConstHandle(xmlDoc_.FirstChildElement("zombieGame")).FirstChildElement("zombie");
		float mass = zombie::extract<float>(zombieTag.FirstChildElement("mass"));
		float radius = zombie::extract<float>(zombieTag.FirstChildElement("radius"));
		float life = zombie::extract<float>(zombieTag.FirstChildElement("life"));
		float walkingSpeed = zombie::extract<float>(zombieTag.FirstChildElement("walkingSpeed"));
		float runningSpeed = zombie::extract<float>(zombieTag.FirstChildElement("runningSpeed"));
		float stamina = zombie::extract<float>(zombieTag.FirstChildElement("stamina"));
		Animation moveA = extractAnimation(zombieTag.FirstChildElement("moveAnimation"));
		Position grip;
		grip.x = zombie::extract<float>(zombieTag.FirstChildElement("moveImageGripX"));
		grip.y = zombie::extract<float>(zombieTag.FirstChildElement("moveImageGripY"));
		Animation injuredA = extractAnimation(zombieTag.FirstChildElement("injuredAnimation"));
		Animation dieA = extractAnimation(zombieTag.FirstChildElement("dieAnimation"));
		mw::Sound dieS = loadSound(zombie::extract<std::string>(zombieTag.FirstChildElement("dieSound")));
		mw::Sound hitS = loadSound(zombie::extract<std::string>(zombieTag.FirstChildElement("hitSound")));
		std::string weaponName = zombie::extract<std::string>(zombieTag.FirstChildElement("weapon"));

		dataInterface.loadZombie(mass, radius, life, walkingSpeed, runningSpeed, stamina, moveA, grip, injuredA, dieA, dieS, hitS, weaponName);
	}

	void GameData::loadMap(DataInterface& dataInterface) const {
		tinyxml2::XMLConstHandle handleXml = tinyxml2::XMLConstHandle(xmlMap_.FirstChildElement("map")).FirstChildElement("objects").FirstChildElement("object");
		while (handleXml.ToElement() != nullptr) {
			if (handleXml.ToElement()->Attribute("type", "building")) {
				std::string geom = zombie::extract<std::string>(handleXml.FirstChildElement("geom"));
				std::stringstream stream(geom);
				std::string word;

				if (stream >> word) { // Assume "POLYGON"
					dataInterface.loadBuilding(loadPolygon(stream.str()));
				}
			} else if (handleXml.ToElement()->Attribute("type", "water")) {
				std::string geom = zombie::extract<std::string>(handleXml.FirstChildElement("geom"));
				std::stringstream stream(geom);
				std::string word;

				if (stream >> word) { // Assume "POLYGON"
					dataInterface.loadWater(loadPolygon(stream.str()));
				}
			} else if (handleXml.ToElement()->Attribute("type", "road")) {
				std::string geom = zombie::extract<std::string>(handleXml.FirstChildElement("geom"));
				std::stringstream stream(geom);
				std::string word;

				if (stream >> word) { // Assume "POLYGON"
					dataInterface.loadRoad(loadPolygon(stream.str()));
				}
			} else if (handleXml.ToElement()->Attribute("type", "tree")) {
				dataInterface.loadTree(zombie::extract<Point>(handleXml.FirstChildElement("geom")));
			} else if (handleXml.ToElement()->Attribute("type", "spawningpoint")) {
				dataInterface.loadSpawningPoint(zombie::extract<Point>(handleXml.FirstChildElement("geom")));
			}

			handleXml = handleXml.NextSiblingElement("object");
		}
	}

	void GameData::loadWeapon(DataInterface& dataInterface) const {
		// Find all weapons.
		tinyxml2::XMLConstHandle weaponTag = tinyxml2::XMLConstHandle(xmlDoc_.FirstChildElement("zombieGame")).FirstChildElement("weapons").FirstChildElement("weapon");
		while (weaponTag.ToElement() != nullptr) {
			std::string name = zombie::extract<std::string>(weaponTag.FirstChildElement("name"));
			mw::Sprite sprite = extractSprite(weaponTag.FirstChildElement("symbolImage"));

			float damage = zombie::extract<float>(weaponTag.FirstChildElement("damage"));
			float timeBetweenShots = zombie::extract<float>(weaponTag.FirstChildElement("timeBetweenShots"));
			float range = zombie::extract<float>(weaponTag.FirstChildElement("range"));
			int clipSize = zombie::extract<int>(weaponTag.FirstChildElement("clipSize"));

			mw::Sound shoot = loadSound(zombie::extract<std::string>(weaponTag.FirstChildElement("shootSound")));
			mw::Sound reload = loadSound(zombie::extract<std::string>(weaponTag.FirstChildElement("reloadSound")));
			Animation animation = extractAnimation(weaponTag.FirstChildElement("moveAnimation"));
			Position grip;
			grip.x = zombie::extract<float>(weaponTag.FirstChildElement("moveImageGripX"));
			grip.y = zombie::extract<float>(weaponTag.FirstChildElement("moveImageGripY"));

			dataInterface.loadWeapon(name, damage, timeBetweenShots, range, clipSize, sprite, animation, grip, shoot, reload);

			weaponTag = weaponTag.NextSiblingElement("weapon");
		}
	}

	void GameData::setWindowSize(int width, int height) {
		tinyxml2::XMLHandle handleXml = tinyxml2::XMLHandle(xmlDoc_.FirstChildElement("zombieGame")).FirstChildElement("window");
		zombie::insert(width, handleXml.FirstChildElement("width"));
		zombie::insert(height, handleXml.FirstChildElement("height"));
		save();
	}

	int GameData::getWindowWidth() const {
		return zombie::getValueFromTag<int>(xmlDoc_, "zombieGame window width");
	}

	int GameData::getWindowHeight() const {
		return zombie::getValueFromTag<int>(xmlDoc_, "zombieGame window height");
	}

	void GameData::setWindowPosition(int x, int y) {
		tinyxml2::XMLHandle handleXml = tinyxml2::XMLHandle(xmlDoc_.FirstChildElement("zombieGame")).FirstChildElement("window");
		zombie::insert(x, handleXml.FirstChildElement("positionX"));
		zombie::insert(y, handleXml.FirstChildElement("positionY"));
		save();
	}

	int GameData::getWindowXPosition() const {
		return zombie::getValueFromTag<int>(xmlDoc_, "zombieGame window positionX");
	}

	int GameData::getWindowYPosition() const {
		return zombie::getValueFromTag<int>(xmlDoc_, "zombieGame window positionY");
	}

	void GameData::setWindowMaximized(bool maximized) {
		tinyxml2::XMLHandle handleXml = tinyxml2::XMLHandle(xmlDoc_.FirstChildElement("zombieGame")).FirstChildElement("window");
		zombie::insert(maximized, handleXml.FirstChildElement("maximized"));
		save();
	}

	bool GameData::isWindowMaximized() const {
		return zombie::getValueFromTag<bool>(xmlDoc_, "zombieGame window maximized");
	}

	float GameData::getImpulseThreshold() const {
		return zombie::getValueFromTag<float>(xmlDoc_, "zombieGame settings impulseThreshold");
	}

	float GameData::getInnerSpawnRadius() const {
		return zombie::getValueFromTag<float>(xmlDoc_, "zombieGame settings innerSpawnRadius");
	}

	float GameData::getOuterSpawnRadius() const {
		return zombie::getValueFromTag<float>(xmlDoc_, "zombieGame settings outerSpawnRadius");
	}

	int GameData::getTimeStepMS() const {
		return zombie::getValueFromTag<int>(xmlDoc_, "zombieGame settings timeStepMS");
	}

	int GameData::getUnitLevel() const {
		return zombie::getValueFromTag<int>(xmlDoc_, "zombieGame settings unitLevel");
	}

	int GameData::getUnitLimit() const {
		return zombie::getValueFromTag<int>(xmlDoc_, "zombieGame settings unitLimit");
	}

	mw::Sprite GameData::getMenuBackgroundImage() const {
		tinyxml2::XMLConstHandle menuTag = tinyxml2::XMLConstHandle(xmlDoc_.FirstChildElement("zombieGame")).FirstChildElement("menu");
		mw::Sprite sprite = extractSprite(menuTag.FirstChildElement("backGroundImage"));
		return sprite;
	}

	mw::Sprite GameData::getTreeImage() const {
		tinyxml2::XMLConstHandle treeTag = tinyxml2::XMLConstHandle(xmlDoc_.FirstChildElement("zombieGame")).FirstChildElement("tree");
		mw::Sprite sprite = extractSprite(treeTag.FirstChildElement("image"));
		return sprite;
	}

	mw::Sprite GameData::getWallImage() const {
		tinyxml2::XMLConstHandle wallTag = tinyxml2::XMLConstHandle(xmlDoc_.FirstChildElement("zombieGame")).FirstChildElement("buildings");
		mw::Sprite sprite = extractSprite(wallTag.FirstChildElement("wallImage"));
		return sprite;
	}

	void GameData::loadFrame(tinyxml2::XMLConstHandle frameTag, Animation& animation) const {
		mw::Sprite sprite = extractSprite(frameTag.FirstChildElement("image"));

		float time = 1;
		try {
			time = zombie::extract<float>(frameTag.FirstChildElement("time"));
		} catch (mw::Exception&) {
			// Time tag probably missing!
			// Do nothing. Default time 1.
		}
		float bodyWidth = zombie::extract<float>(frameTag.FirstChildElement("bodyWidth"));

		animation.add(sprite, bodyWidth, time);
	}

	mw::Sprite GameData::extractSprite(tinyxml2::XMLConstHandle handle) const {
		const tinyxml2::XMLElement* element = handle.ToElement();
		if (element == nullptr) {
			throw mw::Exception("Missing element!");
		}

		const char* str = element->GetText();

		if (str == nullptr) {
			throw mw::Exception("Missing text!");
		}

		float x = element->FloatAttribute("x");
		float y = element->FloatAttribute("y");
		float h = element->FloatAttribute("h");
		float w = element->FloatAttribute("w");

		mw::Texture texture = loadTexture(str);

		if (h < 1) {
			h = (float) texture.getHeight();
		}

		if (w < 1) {
			w = (float) texture.getWidth();
		}

		return mw::Sprite(texture, x, y, w, h);
	}

	Animation GameData::extractAnimation(tinyxml2::XMLConstHandle animationTag) const {
		tinyxml2::XMLConstHandle handle = animationTag.FirstChildElement("frame");
		Animation animation;
		while (handle.ToElement() != nullptr) {
			loadFrame(handle, animation);
			handle = handle.NextSiblingElement("frame");
		}

		return animation;
	}

	mw::Font GameData::loadFont(std::string file, unsigned int fontSize) const {
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

	mw::Texture GameData::loadTexture(std::string file) const {
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

	mw::Sound GameData::loadSound(std::string file) const {
		unsigned int size = sounds_.size();
		mw::Sound& sound = sounds_[file];

		// Sound not found?
		if (sounds_.size() > size) {
			sound = mw::Sound(file);
		}

		return sound;
	}

} // Namespace zombie.
