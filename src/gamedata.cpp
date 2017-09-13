#include "gamedata.h"

#include <fstream>

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
		std::ifstream mapIn(root_["settings"]["map"].asString(), std::ifstream::binary);
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
		return loadFont(root_["window"]["font"].asString(), size);
	}

	void GameData::bindTextureFromAtlas() const {
		textureAtlas_.getTexture().bindTexture();
	}

	int GameData::getWindowPositionX() {
		return root_["window"]["positionX"].asInt();
	}

	int GameData::getWindowPositionY() {
		return root_["window"]["positionY"].asInt();
	}

	void GameData::setWindowPositionX(int x) {
		root_["window"]["positionX"] = x;
	}

	void GameData::setWindowPositionY(int y) {
		root_["window"]["positionY"] = y;
	}

	int GameData::getWindowWidth() {
		return root_["window"]["width"].asInt();
	}

	int GameData::getWindowHeight() {
		return root_["window"]["height"].asInt();
	}

	void GameData::setWindowWidth(int width) {
		root_["window"]["width"] = width;
	}

	void GameData::setWindowHeight(int height) {
		root_["window"]["height"] = height;
	}

	bool GameData::isWindowResizable() {
		return root_["window"]["resizeable"].asBool();
	}

	void GameData::setWindowResizable(bool resizeable) {
		root_["window"]["resizeable"] = resizeable;
	}

	int GameData::getWindowMinWidth() {
		return root_["window"]["minWidth"].asInt();
	}

	int GameData::getWindowMinHeight() {
		return root_["window"]["minHeight"].asInt();
	}

	std::string GameData::getWindowIcon() {
		return root_["window"]["icon"].asString();
	}

	bool GameData::isWindowBordered() {
		return root_["window"]["border"].asBool();
	}

	void GameData::setWindowBordered(bool border) {
		root_["window"]["border"] = border;
	}

	bool GameData::isWindowMaximized() {
		return root_["window"]["maximized"].asBool();
	}

	void GameData::setWindowMaximized(bool maximized) {
		root_["window"]["maximized"] = maximized;
	}

	bool GameData::isWindowVsync() {
		return root_["window"]["vsync"].asBool();
	}

	void GameData::setWindowVsync(bool activate) {
		root_["window"]["vsync"] = activate;
	}

	bool GameData::isMusicOn() {
		return root_["music"]["switch"].asBool();
	}

	void GameData::setMusicOn(bool maximized) {
		root_["music"]["music"] = maximized;
	}

	float GameData::getMusicVolume() {
		return root_["music"]["volume"].asFloat();
	}

	void GameData::setMusicVolume(float volume) {
		root_["music"]["volume"] = volume;
	}

	mw::Music GameData::getMusicTrack() {
		return loadMusic(root_["music"]["track"].asString());
	}

	mw::Sprite GameData::getTreeImage() {
		return loadSprite(root_["tree"].asString());
	}

	mw::Sprite GameData::getBuildingWallImage() {
		return loadSprite(root_["buildings"]["wallImage"].asString());
	}

	// -----------------

	float GameData::getSettingsImpulseThreshold() {
		return root_["settings"]["impulseThreshold"].asFloat();
	}

	float GameData::getSettingsTimeStep() {
		return root_["settings"]["timeStep"].asFloat();
	}

	float GameData::getSettingsInnerSpawnRadius() {
		return root_["settings"]["innerSpawnRadius"].asFloat();
	}

	float GameData::getSettingsOuterSpawnRadius() {
		return root_["settings"]["outerSpawnRadius"].asFloat();
	}

	int GameData::getSettingsUnitLevel() {
		return root_["settings"]["unitLevel"].asInt();
	}

	int GameData::getSettingsUnitLimit() {
		return root_["settings"]["unitLimit"].asInt();
	}

	std::string GameData::getSettingsMap() {
		return root_["settings"]["map"].asString();
	}

	mw::Sound GameData::getMenuSoundChoice() {
		return loadSound(root_["menu"]["soundChoice"].asString());
	}

	mw::Sound GameData::getMenuSoundHighlited() {
		return loadSound(root_["menu"]["soundHighlited"].asString());
	}

	mw::Sprite GameData::getMenuBackgroundImage() {
		return loadSprite(root_["menu"]["backgroundImage"].asString());
	}

	mw::Sprite GameData::getWaterSeeFloorImage() {
		return loadSprite(root_["water"]["seeFloorImage"].asString());
	}

	mw::Sprite GameData::getRoadIntersection() {
		return loadSprite(root_["roads"]["intersection"].asString());
	}

	mw::Sprite GameData::getRoadStraight0() {
		return loadSprite(root_["roads"]["straight0"].asString());
	}

	mw::Sprite GameData::getRoadStraight90() {
		return loadSprite(root_["roads"]["straight90"].asString());
	}

	mw::Sprite GameData::getRoadTurn0() {
		return loadSprite(root_["roads"]["turn0"].asString());
	}

	mw::Sprite GameData::getRoadTurn90() {
		return loadSprite(root_["roads"]["turn90"].asString());
	}

	mw::Sprite GameData::getRoadTurn180() {
		return loadSprite(root_["roads"]["turn180"].asString());
	}

	mw::Sprite GameData::getRoadTurn270() {
		return loadSprite(root_["roads"]["turn270"].asString());
	}

	mw::Sprite GameData::getRoadTurnIntersection0() {
		return loadSprite(root_["roads"]["tintersection0"].asString());
	}

	mw::Sprite GameData::getRoadTurnIntersection90() {
		return loadSprite(root_["roads"]["tintersection90"].asString());
	}

	mw::Sprite GameData::getRoadTurnIntersection180() {
		return loadSprite(root_["roads"]["tintersection180"].asString());
	}

	mw::Sprite GameData::getRoadTurntersection270() {
		return loadSprite(root_["roads"]["tintersection270"].asString());
	}

	ExplosionProperties GameData::getExplosionProperties() {
		float timeDelay = root_["explosion"]["timeDelay"].asFloat();
		float speed = root_["explosion"]["speed"].asFloat();
		mw::Sound sound = loadSound(root_["explosion"]["sound"].asString());
		Animation animation = loadAnimation(root_["explosion"]["animation"]);
		animation.setLooping(false);
		return ExplosionProperties(animation, sound, timeDelay);
	}

	UnitProperties GameData::getHumanProperties() {
		bool test = root_.isMember("human");
		return loadUnitProperties(root_["human"]);
	}

	UnitProperties GameData::getZombieProperties() {
		return loadUnitProperties(root_["zombie"]);
	}

	Animation GameData::loadAnimation(Json::Value animationTag) {
		float defaultDeltaTime = 1.f;
		if (animationTag.isMember("deltaTime")) {
			defaultDeltaTime = animationTag["deltaTime"].asFloat();
		}

		float defaultBodyWidth = 1.f;
		if (animationTag.isMember("bodyWidth")) {
			defaultBodyWidth = animationTag["bodyWidth"].asFloat();
		}

		Animation animation;
		for (Json::Value& child : animationTag["frames"]) {
			float bodyWidth = defaultBodyWidth;
			if (child.isMember("bodyWidth")) {
				bodyWidth = child["bodyWidth"].asFloat();
			}
			float deltaTime = defaultDeltaTime;
			if (child.isMember("time")) {
				deltaTime = child["time"].asFloat();
			}
			auto n = child["image"].asString();
			animation.add(loadSprite(child["image"].asString()), bodyWidth, deltaTime);
		}
		return animation;
	}

	MissileProperties GameData::getMissileProperties() {
		return loadMissileProperties(std::string("missile"));
	}

	UnitProperties GameData::loadUnitProperties(Json::Value unitTag) {
		UnitProperties unitProperties;
		unitProperties.hitSound_ = loadSound(unitTag["hitSound"].asString());
		unitProperties.dieSound_ = loadSound(unitTag["dieSound"].asString());
		unitProperties.moveAnimation_ = loadAnimation(unitTag["moveAnimation"]);
		unitProperties.dieAnimation_ = loadAnimation(unitTag["dieAnimation"]);
		unitProperties.injuredAnimation_ = loadAnimation(unitTag["injuredAnimation"]);
		unitProperties.grip_ = Position(unitTag["moveImageGripX"].asFloat(), unitTag["moveImageGripY"].asFloat());
		unitProperties.mass_ = unitTag["mass"].asFloat();
		unitProperties.radius_ = unitTag["radius"].asFloat();
		unitProperties.life_ = unitTag["life"].asFloat();
		unitProperties.walkingSpeed_ = unitTag["walkingSpeed"].asFloat();
		unitProperties.runningSpeed_ = unitTag["runningSpeed"].asFloat();
		unitProperties.stamina_ = unitTag["stamina"].asFloat();
		unitProperties.weaponProperties_ = loadWeaponProperties(unitTag["weapon"].asString());
		return unitProperties;
	}

	WeaponProperties GameData::loadWeaponProperties(std::string weaponName) {
		for (Json::Value& child : root_["weapons"]) {
			std::string name = child["name"].asString();
			if (name == weaponName) {
				return loadWeaponProperties(child);
			}
		}
		return WeaponProperties();
	}

	WeaponProperties GameData::loadWeaponProperties(Json::Value unitTag) {
		WeaponProperties properties;
		properties.name_ = unitTag["name"].asString();
		properties.type_ = WeaponProperties::BULLET;
		std::string s = unitTag["type"].asString();
		if (unitTag["type"].asString() == "BULLET") {
			properties.type_ = WeaponProperties::BULLET;
		} else if (unitTag["type"].asString() == "MISSILE") {
			properties.type_ = WeaponProperties::MISSILE;
		} else {
			throw 1;
		}
		properties.symbolImage_ = loadSprite(unitTag["symbolImage"].asString());
		properties.timeBetweenShots_ = unitTag["timeBetweenShots"].asFloat();
		properties.clipSize_ = unitTag["clipSize"].asInt();
		properties.shootSound_ = loadSound(unitTag["shootSound"].asString());
		properties.reloadSound_ = loadSound(unitTag["reloadSound"].asString());
		properties.moveAnimation_ = loadAnimation(unitTag["moveAnimation"]);
		properties.size_ = unitTag["size"].asFloat();
		properties.moveImageGrip_ = Position(unitTag["moveImageGripX"].asFloat(), unitTag["moveImageGripY"].asFloat());
		
		// Projectile properties.
		properties.damage_ = unitTag["projectile"]["damage"].asFloat();
		properties.range_ = unitTag["projectile"]["range"].asFloat();
		properties.laserSight_ = unitTag["projectile"]["force"].asBool();
		switch (properties.type_) {
			case WeaponProperties::BULLET:
				break;
			case WeaponProperties::MISSILE:
				properties.damageRadius_ = unitTag["projectile"]["damageRadius"].asFloat();
				properties.deathTime_ = unitTag["projectile"]["deathTime"].asFloat();
				properties.speed_ = unitTag["projectile"]["speed"].asFloat();
				properties.force_ = unitTag["projectile"]["force"].asFloat();
				properties.missileProperties_ = loadMissileProperties(std::string("missile"));
				break;
		}
		
		return properties;
	}

	MissileProperties GameData::loadMissileProperties(std::string missileName) {
		for (Json::Value& child : root_["missiles"]) {
			std::string name = child["name"].asString();
			if (name == missileName) {
				return loadMissileProperties(child);
			}
		}
		return MissileProperties();
	}

	MissileProperties GameData::loadMissileProperties(Json::Value unitTag) {
		MissileProperties properties;
		properties.name_ = unitTag["name"].asString();
		properties.mass_ = unitTag["mass"].asFloat();
		properties.width_ = unitTag["width"].asFloat();
		properties.length_ = unitTag["length"].asFloat();
		properties.animation_ = loadAnimation(unitTag["animation"]);
		properties.moveSound_ = loadSound(unitTag["moveSound"].asString());
		return properties;
	}

	MapProperties GameData::loadMapProperties() {
		MapProperties properties;
		properties.name_ = rootMap_["name"].asString();

		for (Json::Value& child : rootMap_["objects"]) {
			std::string objectType = child["objectType"].asString();
			if (objectType == "building") {
				MapProperties::Object ob;
				ob.type_ = MapProperties::BUILDING;
				ob.geom_ = loadPolygon(child["geom"].asString());
				properties.positions_.push_back(ob);
			} else if (objectType == "grass") {
				MapProperties::Object ob;
				ob.type_ = MapProperties::GRASS;
				ob.geom_ = loadPolygon(child["geom"].asString());
				properties.positions_.push_back(ob);
			} else if (objectType == "tilepoint") {
				MapProperties::Object ob;
				ob.type_ = MapProperties::TILEPOINT;
				ob.geom_.push_back(loadPoint(child["geom"].asString()));
				properties.positions_.push_back(ob);
			} else if (objectType == "tree") {
				MapProperties::Object ob;
				ob.type_ = MapProperties::TREE;
				ob.geom_.push_back(loadPoint(child["geom"].asString()));
				properties.positions_.push_back(ob);
			} else if (objectType == "water") {
				MapProperties::Object ob;
				ob.type_ = MapProperties::WATER;
				ob.geom_ = loadPolygon(child["geom"].asString());
				properties.positions_.push_back(ob);
			} else if (objectType == "spawningpoint") {
				MapProperties::Object ob;
				ob.type_ = MapProperties::SPAWNINGPOINT;
				ob.geom_.push_back(loadPoint(child["geom"].asString()));
				properties.positions_.push_back(ob);
			} else if (objectType == "car spawningpoint") {
				MapProperties::Object ob;
				ob.type_ = MapProperties::CAR_SPAWNINGPOINT;
				ob.geom_.push_back(loadPoint(child["geom"].asString()));
				properties.positions_.push_back(ob);
			}
		}
		return properties;
	}

} // Namespace zombie.
