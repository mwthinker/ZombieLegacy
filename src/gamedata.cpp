#include "gamedata.h"

#include <fstream>
#include <sstream>

using nlohmann::json;

namespace zombie {

	namespace {
		
		// Takes a string as input and returns the point.
		// The string "POINT (x y)" the input should be defined
		// as "POINT (...)".
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
	
		// Takes a string as input and returns the points.
		// The string "POLYGON ((x1 y1, x2 y2, ...))" the input should be defined
		// as "POLYGON ((...))". The last point is assumed to be the same as the first, therefore
		// the last point will not be includeded in the returned vector.
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
	
	}

	GameData::GameData() : textureAtlas_(2048, 2048, []() {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}) {
		std::ifstream in(JSON_PATH, std::ifstream::binary);
		in >> root_;
		std::ifstream mapIn(root_["settings"]["map"].get<std::string>(), std::ifstream::binary);
		mapIn >> rootMap_;
	}

	void GameData::save() {
		std::ofstream out(JSON_PATH);
		//root_ >> out;
	}

	mw::Font GameData::loadFont(std::string file, unsigned int fontSize) {
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

	mw::Sound GameData::loadSound(std::string file) {
		unsigned int size = sounds_.size();
		mw::Sound& sound = sounds_[file];

		// Sound not found?
		if (sounds_.size() > size) {
			sound = mw::Sound(file);
		}

		return sound;
	}

	mw::Music GameData::loadMusic(std::string file) {
		unsigned int size = musics_.size();
		mw::Music& music = musics_[file];

		// Music not found?
		if (musics_.size() > size) {
			music = mw::Music(file);
		}

		return music;
	}

	mw::Sprite GameData::loadSprite(std::string file) {
		return textureAtlas_.add(file, 1);;
	}

	mw::Font GameData::getDefaultFont(int size) {
		return loadFont(root_["window"]["font"].get<std::string>(), size);
	}

	void GameData::bindTextureFromAtlas() const {
		textureAtlas_.getTexture().bindTexture();
	}

	int GameData::getWindowPositionX() {
		return root_["window"]["positionX"].get<int>();
	}

	int GameData::getWindowPositionY() {
		return root_["window"]["positionY"].get<int>();
	}

	void GameData::setWindowPositionX(int x) {
		root_["window"]["positionX"] = x;
	}

	void GameData::setWindowPositionY(int y) {
		root_["window"]["positionY"] = y;
	}

	int GameData::getWindowWidth() {
		return root_["window"]["width"].get<int>();
	}

	int GameData::getWindowHeight() {
		return root_["window"]["height"].get<int>();
	}

	void GameData::setWindowWidth(int width) {
		root_["window"]["width"] = width;
	}

	void GameData::setWindowHeight(int height) {
		root_["window"]["height"] = height;
	}

	bool GameData::isWindowResizable() {
		return root_["window"]["resizeable"].get<bool>();
	}

	void GameData::setWindowResizable(bool resizeable) {
		root_["window"]["resizeable"] = resizeable;
	}

	int GameData::getWindowMinWidth() {
		return root_["window"]["minWidth"].get<int>();
	}

	int GameData::getWindowMinHeight() {
		return root_["window"]["minHeight"].get<int>();
	}

	std::string GameData::getWindowIcon() {
		return root_["window"]["icon"].get<std::string>();
	}

	bool GameData::isWindowBordered() {
		return root_["window"]["border"].get<bool>();
	}

	void GameData::setWindowBordered(bool border) {
		root_["window"]["border"] = border;
	}

	bool GameData::isWindowMaximized() {
		return root_["window"]["maximized"].get<bool>();
	}

	void GameData::setWindowMaximized(bool maximized) {
		root_["window"]["maximized"] = maximized;
	}

	bool GameData::isWindowVsync() {
		return root_["window"]["vsync"].get<bool>();
	}

	void GameData::setWindowVsync(bool activate) {
		root_["window"]["vsync"] = activate;
	}

	bool GameData::isMusicOn() {
		return root_["music"]["switch"].get<bool>();
	}

	void GameData::setMusicOn(bool maximized) {
		root_["music"]["music"] = maximized;
	}

	float GameData::getMusicVolume() {
		return root_["music"]["volume"].get<float>();
	}

	void GameData::setMusicVolume(float volume) {
		root_["music"]["volume"] = volume;
	}

	mw::Music GameData::getMusicTrack() {
		return loadMusic(root_["music"]["track"].get<std::string>());
	}

	mw::Sprite GameData::getTreeImage() {
		return loadSprite(root_["tree"].get<std::string>());
	}

	mw::Sprite GameData::getBuildingWallImage() {
		return loadSprite(root_["buildings"]["wallImage"].get<std::string>());
	}

	// -----------------

	float GameData::getSettingsImpulseThreshold() {
		return root_["settings"]["impulseThreshold"].get<float>();
	}

	float GameData::getSettingsTimeStep() {
		return root_["settings"]["timeStep"].get<float>();
	}

	float GameData::getSettingsInnerSpawnRadius() {
		return root_["settings"]["innerSpawnRadius"].get<float>();
	}

	float GameData::getSettingsOuterSpawnRadius() {
		return root_["settings"]["outerSpawnRadius"].get<float>();
	}

	int GameData::getSettingsUnitLevel() {
		return root_["settings"]["unitLevel"].get<int>();
	}

	int GameData::getSettingsUnitLimit() {
		return root_["settings"]["unitLimit"].get<int>();
	}

	std::string GameData::getSettingsMap() {
		return root_["settings"]["map"].get<std::string>();
	}

	mw::Sound GameData::getMenuSoundChoice() {
		return loadSound(root_["menu"]["soundChoice"].get<std::string>());
	}

	mw::Sound GameData::getMenuSoundHighlited() {
		return loadSound(root_["menu"]["soundHighlited"].get<std::string>());
	}

	mw::Sprite GameData::getMenuBackgroundImage() {
		return loadSprite(root_["menu"]["backgroundImage"].get<std::string>());
	}

	mw::Sprite GameData::getWaterSeeFloorImage() {
		return loadSprite(root_["water"]["seeFloorImage"].get<std::string>());
	}

	mw::Sprite GameData::getRoadIntersection() {
		return loadSprite(root_["roads"]["intersection"].get<std::string>());
	}

	mw::Sprite GameData::getRoadStraight0() {
		return loadSprite(root_["roads"]["straight0"].get<std::string>());
	}

	mw::Sprite GameData::getRoadStraight90() {
		return loadSprite(root_["roads"]["straight90"].get<std::string>());
	}

	mw::Sprite GameData::getRoadTurn0() {
		return loadSprite(root_["roads"]["turn0"].get<std::string>());
	}

	mw::Sprite GameData::getRoadTurn90() {
		return loadSprite(root_["roads"]["turn90"].get<std::string>());
	}

	mw::Sprite GameData::getRoadTurn180() {
		return loadSprite(root_["roads"]["turn180"].get<std::string>());
	}

	mw::Sprite GameData::getRoadTurn270() {
		return loadSprite(root_["roads"]["turn270"].get<std::string>());
	}

	mw::Sprite GameData::getRoadTurnIntersection0() {
		return loadSprite(root_["roads"]["tintersection0"].get<std::string>());
	}

	mw::Sprite GameData::getRoadTurnIntersection90() {
		return loadSprite(root_["roads"]["tintersection90"].get<std::string>());
	}

	mw::Sprite GameData::getRoadTurnIntersection180() {
		return loadSprite(root_["roads"]["tintersection180"].get<std::string>());
	}

	mw::Sprite GameData::getRoadTurntersection270() {
		return loadSprite(root_["roads"]["tintersection270"].get<std::string>());
	}

	ExplosionProperties GameData::getExplosionProperties() {
		float timeDelay = root_["explosion"]["timeDelay"].get<float>();
		float speed = root_["explosion"]["speed"].get<float>();
		mw::Sound sound = loadSound(root_["explosion"]["sound"].get<std::string>());
		Animation animation = loadAnimation(root_["explosion"]["animation"]);
		animation.setLooping(false);
		return ExplosionProperties(animation, sound, timeDelay);
	}

	UnitProperties GameData::getHumanProperties() {
		return loadUnitProperties(root_["human"]);
	}

	UnitProperties GameData::getZombieProperties() {
		return loadUnitProperties(root_["zombie"]);
	}

	Animation GameData::loadAnimation(const json& animationTag) {
		float defaultDeltaTime = 1.f;
		try {
			defaultDeltaTime = animationTag.at("deltaTime").get<float>();
		} catch (nlohmann::detail::out_of_range) {
			// Do nothing.
		}

		float defaultBodyWidth = 1.f;
		try {
			defaultBodyWidth = animationTag.at("bodyWidth").get<float>();
		} catch (nlohmann::detail::out_of_range) {
			// Do nothing.
		}

		Animation animation;
		try {
			for (const auto& child : animationTag.at("frames")) {
				float bodyWidth = defaultBodyWidth;
				try {
					bodyWidth = child.at("bodyWidth").get<float>();
				} catch (nlohmann::detail::out_of_range) {
					// Do nothing.
				}
			
				float deltaTime = defaultDeltaTime;
				try {
					deltaTime = child.at("time").get<float>();
				} catch (nlohmann::detail::out_of_range) {
					// Do nothing.
				}

				auto n = child["image"].get<std::string>();
				animation.add(loadSprite(child["image"].get<std::string>()), bodyWidth, deltaTime);
			}
		} catch (nlohmann::detail::out_of_range) {
			// Do nothing.
		}

		return animation;
	}

	MissileProperties GameData::getMissileProperties() {
		return loadMissileProperties(std::string("missile"));
	}

	UnitProperties GameData::loadUnitProperties(const json& unitTag) {
		UnitProperties unitProperties;
		unitProperties.hitSound_ = loadSound(unitTag["hitSound"].get<std::string>());
		unitProperties.dieSound_ = loadSound(unitTag["dieSound"].get<std::string>());
		unitProperties.moveAnimation_ = loadAnimation(unitTag["moveAnimation"]);
		unitProperties.dieAnimation_ = loadAnimation(unitTag["dieAnimation"]);
		unitProperties.injuredAnimation_ = loadAnimation(unitTag["injuredAnimation"]);
		unitProperties.grip_ = Position(unitTag["moveImageGripX"].get<float>(), unitTag["moveImageGripY"].get<float>());
		unitProperties.mass_ = unitTag["mass"].get<float>();
		unitProperties.radius_ = unitTag["radius"].get<float>();
		unitProperties.life_ = unitTag["life"].get<float>();
		unitProperties.walkingSpeed_ = unitTag["walkingSpeed"].get<float>();
		unitProperties.runningSpeed_ = unitTag["runningSpeed"].get<float>();
		unitProperties.stamina_ = unitTag["stamina"].get<float>();
		unitProperties.weaponProperties_ = loadWeaponProperties(unitTag["weapon"].get<std::string>());
		return unitProperties;
	}

	WeaponProperties GameData::loadWeaponProperties(std::string weaponName) {
		for (const auto& child : root_["weapons"]) {
			std::string name = child["name"].get<std::string>();
			if (name == weaponName) {
				return loadWeaponProperties(child);
			}
		}
		return WeaponProperties();
	}

	WeaponProperties GameData::loadWeaponProperties(const json& unitTag) {
		WeaponProperties properties;
		properties.name_ = unitTag["name"].get<std::string>();
		properties.type_ = WeaponProperties::BULLET;
		std::string s = unitTag["type"].get<std::string>();
		if (unitTag["type"].get<std::string>() == "BULLET") {
			properties.type_ = WeaponProperties::BULLET;
		} else if (unitTag["type"].get<std::string>() == "MISSILE") {
			properties.type_ = WeaponProperties::MISSILE;
		} else {
			throw 1;
		}
		properties.symbolImage_ = loadSprite(unitTag["symbolImage"].get<std::string>());
		properties.timeBetweenShots_ = unitTag["timeBetweenShots"].get<float>();
		properties.clipSize_ = unitTag["clipSize"].get<int>();
		properties.shootSound_ = loadSound(unitTag["shootSound"].get<std::string>());
		properties.reloadSound_ = loadSound(unitTag["reloadSound"].get<std::string>());
		properties.moveAnimation_ = loadAnimation(unitTag["moveAnimation"]);
		properties.size_ = unitTag["size"].get<float>();
		properties.moveImageGrip_ = Position(unitTag["moveImageGripX"].get<float>(), unitTag["moveImageGripY"].get<float>());
		
		// Projectile properties.
		properties.damage_ = unitTag["projectile"]["damage"].get<float>();
		properties.range_ = unitTag["projectile"]["range"].get<float>();
		properties.laserSight_ = unitTag["laserSight"].get<bool>();
		switch (properties.type_) {
			case WeaponProperties::BULLET:
				break;
			case WeaponProperties::MISSILE:
				properties.damageRadius_ = unitTag["projectile"]["damageRadius"].get<float>();
				properties.deathTime_ = unitTag["projectile"]["deathTime"].get<float>();
				properties.speed_ = unitTag["projectile"]["speed"].get<float>();
				properties.force_ = unitTag["projectile"]["force"].get<float>();
				properties.missileProperties_ = loadMissileProperties(std::string("missile"));
				break;
		}
		
		return properties;
	}

	MissileProperties GameData::loadMissileProperties(std::string missileName) {
		for (const auto& child : root_["missiles"]) {
			std::string name = child["name"].get<std::string>();
			if (name == missileName) {
				return loadMissileProperties(child);
			}
		}
		return MissileProperties();
	}

	MissileProperties GameData::loadMissileProperties(const json& unitTag) {
		MissileProperties properties;
		properties.name_ = unitTag["name"].get<std::string>();
		properties.mass_ = unitTag["mass"].get<float>();
		properties.width_ = unitTag["width"].get<float>();
		properties.length_ = unitTag["length"].get<float>();
		properties.animation_ = loadAnimation(unitTag["animation"]);
		properties.moveSound_ = loadSound(unitTag["moveSound"].get<std::string>());
		return properties;
	}

	MapProperties GameData::loadMapProperties() {
		MapProperties properties;
		properties.name_ = rootMap_["name"].get<std::string>();

		for (const auto& child : rootMap_["objects"]) {
			std::string objectType = child["objectType"].get<std::string>();
			if (objectType == "building") {
				MapProperties::Object ob;
				ob.type_ = MapProperties::BUILDING;
				ob.geom_ = loadPolygon(child["geom"].get<std::string>());
				properties.positions_.push_back(ob);
			} else if (objectType == "grass") {
				MapProperties::Object ob;
				ob.type_ = MapProperties::GRASS;
				ob.geom_ = loadPolygon(child["geom"].get<std::string>());
				properties.positions_.push_back(ob);
			} else if (objectType == "tilepoint") {
				MapProperties::Object ob;
				ob.type_ = MapProperties::TILEPOINT;
				ob.geom_.push_back(loadPoint(child["geom"].get<std::string>()));
				properties.positions_.push_back(ob);
			} else if (objectType == "tree") {
				MapProperties::Object ob;
				ob.type_ = MapProperties::TREE;
				ob.geom_.push_back(loadPoint(child["geom"].get<std::string>()));
				properties.positions_.push_back(ob);
			} else if (objectType == "water") {
				MapProperties::Object ob;
				ob.type_ = MapProperties::WATER;
				ob.geom_ = loadPolygon(child["geom"].get<std::string>());
				properties.positions_.push_back(ob);
			} else if (objectType == "spawningpoint") {
				MapProperties::Object ob;
				ob.type_ = MapProperties::SPAWNINGPOINT;
				ob.geom_.push_back(loadPoint(child["geom"].get<std::string>()));
				properties.positions_.push_back(ob);
			} else if (objectType == "car spawningpoint") {
				MapProperties::Object ob;
				ob.type_ = MapProperties::CAR_SPAWNINGPOINT;
				ob.geom_.push_back(loadPoint(child["geom"].get<std::string>()));
				properties.positions_.push_back(ob);
			}
		}
		return properties;
	}

} // Namespace zombie.
