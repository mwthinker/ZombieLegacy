#include "gamedata.h"
#include "load.h"
#include "building2d.h"

#include <mw/sound.h>
#include <mw/texture.h>
#include <mw/exception.h>

#include <tinyxml2.h>

#include <map>
#include <string>

namespace zombie {

	namespace {

		std::vector<WeaponProperties> loadWeapons(tinyxml2::XMLHandle weaponsTag) {
			std::vector<WeaponProperties> weapons;

			// Find all weapons.
			tinyxml2::XMLElement* element = weaponsTag.FirstChildElement("weapon").ToElement();
			while (element != nullptr) {
				WeaponProperties properties;
				properties.name_ = convertFromText<const char*>(toText(element->FirstChildElement("name")));
				properties.image_ = convertFromText<const char*>(toText(element->FirstChildElement("image")));
				properties.damage_ = convertFromText<float>(toText(element->FirstChildElement("damage")));
				properties.timeBetweenShots_ = convertFromText<float>(toText(element->FirstChildElement("timeBetweenShots")));
				properties.range_ = convertFromText<float>(toText(element->FirstChildElement("range")));
				properties.clipSize_ = convertFromText<int>(toText(element->FirstChildElement("clipSize")));
				properties.shootSound_ = convertFromText<const char*>(toText(element->FirstChildElement("shootSound")));
				properties.reloadSound_ = convertFromText<const char*>(toText(element->FirstChildElement("reloadSound")));

				// Add weapon.
				weapons.push_back(properties);

				element = element->NextSiblingElement("weapon");
			}

			return weapons;
		}

		void loadMapItems(tinyxml2::XMLHandle objectsTag, std::vector<Building2D*>& buildings, Terrain2D& terrain) {
			tinyxml2::XMLElement* element = objectsTag.FirstChildElement("object").ToElement();
			while (element != nullptr) {
				if (element->Attribute("type", "building")) {
					std::string geom = convertFromText<const char*>(toText(element->FirstChildElement("geom")));
					std::stringstream stream(geom);
					std::string word;

					if (stream >> word) {
						if (word == "POLYGON") {
							buildings.push_back(new Building2D(loadPolygon(stream.str())));
						}
					}
				} else if (element->Attribute("type", "water")) {
					std::string geom = convertFromText<const char*>(toText(element->FirstChildElement("geom")));
					std::stringstream stream(geom);
					std::string word;

					if (stream >> word) {
						if (word == "POLYGON") {
							terrain.addWater(loadPolygon(stream.str()));
						}
					}
				} else if (element->Attribute("type", "road")) {
					std::string geom = convertFromText<const char*>(toText(element->FirstChildElement("geom")));
					std::stringstream stream(geom);
					std::string word;

					if (stream >> word) {
						if (word == "POLYGON") {
							terrain.addRoad(loadPolygon(stream.str()));
						}
					}
				} else if (element->Attribute("type", "roadline")) {
					std::string geom = convertFromText<const char*>(toText(element->FirstChildElement("geom")));
					std::stringstream stream(geom);
					std::string word;

					if (stream >> word) {
						if (word == "POLYGON") {
							terrain.addRoadLine(loadPolygon(stream.str()));
						}
					}
				}

				element = element->NextSiblingElement("object");
			}
		}

		Settings loadSettings(tinyxml2::XMLHandle settingsTag) {
			Settings settings;
			settings.width_ = convertFromText<int>(toText(settingsTag.FirstChildElement("width")));
			settings.height_ = convertFromText<int>(toText(settingsTag.FirstChildElement("height")));
			settings.unitLevel_ = convertFromText<int>(toText(settingsTag.FirstChildElement("unitLevel")));
			settings.zoomLevel_ = convertFromText<int>(toText(settingsTag.FirstChildElement("zoomLevel")));
			settings.impulseThreshold_ = convertFromText<float>(toText(settingsTag.FirstChildElement("impulseThreshold")));
			settings.timeStepMS_ = convertFromText<int>(toText(settingsTag.FirstChildElement("timeStepMS")));
			settings.mapFile_ = convertFromText<const char*>(toText(settingsTag.FirstChildElement("map")));
			return settings;
		}

		std::vector<UnitProperties> loadUnits(tinyxml2::XMLHandle movingUnitsTag) {
			std::vector<UnitProperties> units;
			// Find all units.
			tinyxml2::XMLElement* element = movingUnitsTag.FirstChildElement("unit").ToElement();
			while (element != nullptr) {
				UnitProperties properties;
				properties.name_ = convertFromText<const char*>(toText(element->FirstChildElement("name")));
				properties.mass_ = convertFromText<float>(toText(element->FirstChildElement("mass")));
				properties.radius_ = convertFromText<float>(toText(element->FirstChildElement("radius")));
				properties.life_ = convertFromText<float>(toText(element->FirstChildElement("life")));
				properties.infected_ = convertFromText<bool>(toText(element->FirstChildElement("infected")));
				properties.walkingSpeed_ = convertFromText<float>(toText(element->FirstChildElement("walkingSpeed")));
				properties.runningSpeed_ = convertFromText<float>(toText(element->FirstChildElement("runningSpeed")));
				properties.stamina_ = convertFromText<float>(toText(element->FirstChildElement("stamina")));
				properties.animation_ = loadAnimation(toElement(element->FirstChildElement("animation")));
				units.push_back(properties);
				element = element->NextSiblingElement("unit");
			}
			return units;
		}

		std::vector<CarProperties> loadCars(tinyxml2::XMLHandle movingUnitsTag) {
			std::vector<CarProperties> cars;
			// Find all cars.
			tinyxml2::XMLElement* element = movingUnitsTag.FirstChildElement("car").ToElement();
			while (element != nullptr) {
				CarProperties properties;
				properties.name_ = convertFromText<const char*>(toText(element->FirstChildElement("name")));
				properties.image_ = convertFromText<const char*>(toText(element->FirstChildElement("image")));
				properties.mass_ = convertFromText<float>(toText(element->FirstChildElement("mass")));
				properties.width_ = convertFromText<float>(toText(element->FirstChildElement("width")));
				properties.length_ = convertFromText<float>(toText(element->FirstChildElement("length")));
				properties.life_ = convertFromText<float>(toText(element->FirstChildElement("life")));
				cars.push_back(properties);
				element = element->NextSiblingElement("car");
			}

			return cars;
		}

	}

	// Returns a random postion between the defined outer and inner circle centered in position.
	Position generatePosition(Position position, float innerRadius, float outerRadius) {
		return position + (innerRadius + (outerRadius - innerRadius) * random()) * Position(std::cosf(random()*2.f*3.14f), std::sinf(random()*2.f*3.14f));
	}

	GameData::GameData(std::string dataFile) {
		// Load XML file.
		tinyxml2::XMLDocument xmlDoc;
		xmlDoc.LoadFile(dataFile.c_str());
		if (xmlDoc.Error()) {
			// Failed!
			xmlDoc.PrintError();
		}
		tinyxml2::XMLHandle handleXml(xmlDoc.FirstChildElement("zombie"));
		tinyxml2::XMLElement* element = handleXml.FirstChildElement("settings").FirstChildElement("width").ToElement();
		std::stringstream stream;
		stream << element->GetText() << " ";
		element = handleXml.FirstChildElement("settings").FirstChildElement("height").ToElement();
		stream << element->GetText() << " ";

		int width, height;
		stream >> width >> height;

		if (width < 0) {
			width = 400;
		}

		if (height < 0) {
			height = 400;
		}

		// Load game data and map data.
		load(handleXml);
	}
	
	void GameData::humanPlayer(std::function<void(State, UnitProperties, const Animation& animation)> func) {
		UnitProperties humanP = units_["Human"];

		Animation animation;
		for (auto tuple : humanP.animation_) {
			animation.add(getLoadedTexture(std::get<0>(tuple)), std::get<2>(tuple));
			animation.setScale(std::get<1>(tuple));
		}

		State state(generatePosition(ORIGO, 0, 50), ORIGO, 0);
		func(state, humanP, animation);
	}

	void GameData::iterateUnits(std::function<void(State, UnitProperties, const Animation& animation)> func) {
		UnitProperties zombieP = units_["Zombie"];

		for (int i = 0; i < settings_.unitLevel_; ++i) {
			Animation animation;
			for (auto tuple : zombieP.animation_) {
				animation.add(getLoadedTexture(std::get<0>(tuple)), std::get<2>(tuple));
				animation.setScale(std::get<1>(tuple));
			}

			State state(generatePosition(ORIGO, 0, 50), ORIGO, 0);
			func(state, zombieP, animation);
		}
	}

	void GameData::iterateCars(std::function<void(State, CarProperties, const mw::Sprite&)> func) {
		CarProperties volvoP = cars_["Volvo"];

		// Add cars.
		for (int i = 0; i < 10; ++i) {
			State state(generatePosition(ORIGO, 0, 50), ORIGO, 0);
			func(state, volvoP, getLoadedTexture(volvoP.image_));
		}
	}

	void GameData::iterateBuildings(std::function<void(Building2D* building)> func) {
		for (Building2D* p : buildings_) {
			func(p);
		}
	}

	bool GameData::load(tinyxml2::XMLHandle xml) {
		try {
			settings_ = loadSettings(xml.FirstChildElement("settings"));
			auto weapons = loadWeapons(xml.FirstChildElement("weapons"));
			for (const WeaponProperties& p : weapons) {
				weapons_[p.name_] = p;
			}
			auto units = loadUnits(xml.FirstChildElement("movingObjects"));
			for (const UnitProperties& p : units) {
				units_[p.name_] = p;
			}
			auto cars = loadCars(xml.FirstChildElement("movingObjects"));
			for (const CarProperties& p : cars) {
				cars_[p.name_] = p;
			}
			// Load map.
			loadMap(settings_.mapFile_);

			return true;
		} catch (mw::Exception&) {
			return false;
		}
	}

	void GameData::loadMap(std::string map) {
		try {
			// Load map file.
			tinyxml2::XMLDocument mapXml;
			mapXml.LoadFile("map.xml");
			if (mapXml.Error()) {
				// Failed!
			}

			tinyxml2::XMLHandle mapHandle(mapXml);
			mapHandle = mapHandle.FirstChildElement("map");

			loadMapItems(mapHandle.FirstChildElement("objects"), buildings_, terrain2d_);
			
		} catch (std::exception&) {
			std::exit(1);
		}
	}

	mw::TexturePtr GameData::getLoadedTexture(std::string file) {
		unsigned int size = textures_.size();

		mw::TexturePtr& texture = textures_[file];
		// Image not found?
		if (textures_.size() > size) {
			texture = mw::TexturePtr(new mw::Texture(file));

			// Image not valid?
			if (!texture->isValid()) {
				// Return null pointer.
				texture = mw::TexturePtr();
			}
		}

		return texture;
	}

} // Namespace zombie.
