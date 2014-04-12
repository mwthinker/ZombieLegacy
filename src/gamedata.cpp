#include "gamedata.h"
#include "load.h"
#include "building2d.h"
#include "car2d.h"
#include "unit2d.h"
#include "weaponitem2d.h"

#include <mw/color.h>
#include <mw/sound.h>
#include <mw/texture.h>
#include <mw/exception.h>

#include <tinyxml2.h>

#include <map>
#include <string>

namespace zombie {

	namespace {

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

	void GameData::loadCars(tinyxml2::XMLHandle movingUnitsTag) {
		// Find all cars.
		tinyxml2::XMLElement* element = movingUnitsTag.FirstChildElement("car").ToElement();
		while (element != nullptr) {
			std::string name = convertFromText<const char*>(toText(element->FirstChildElement("name")));
			mw::Texture texture = loadTexture(convertFromText<const char*>(toText(element->FirstChildElement("image"))));
			
			float mass = convertFromText<float>(toText(element->FirstChildElement("mass")));
			float width = convertFromText<float>(toText(element->FirstChildElement("width")));
			float length = convertFromText<float>(toText(element->FirstChildElement("length")));
			float life = convertFromText<float>(toText(element->FirstChildElement("life")));

			cars_[name] = new Car2D(mass, life, width, length, texture);

			element = element->NextSiblingElement("car");
		}
	}

	void GameData::loadUnits(tinyxml2::XMLHandle movingUnitsTag) {
		// Find all units.
		tinyxml2::XMLElement* element = movingUnitsTag.FirstChildElement("unit").ToElement();
		while (element != nullptr) {
			std::string name = convertFromText<const char*>(toText(element->FirstChildElement("name")));
			float mass = convertFromText<float>(toText(element->FirstChildElement("mass")));
			float radius = convertFromText<float>(toText(element->FirstChildElement("radius")));
			float life = convertFromText<float>(toText(element->FirstChildElement("life")));
			bool infected = convertFromText<bool>(toText(element->FirstChildElement("infected")));
			float walkingSpeed = convertFromText<float>(toText(element->FirstChildElement("walkingSpeed")));
			float runningSpeed = convertFromText<float>(toText(element->FirstChildElement("runningSpeed")));
			float stamina = convertFromText<float>(toText(element->FirstChildElement("stamina")));
			Animation animation = loadAnimation(toElement(element->FirstChildElement("animation")));
			std::string weaponName = convertFromText<const char*>(toText(element->FirstChildElement("weapon")));
			units_[name] = new Unit2D(mass, radius, life, walkingSpeed, runningSpeed, infected, weapons_[weaponName]->getWeapon(), animation);

			element = element->NextSiblingElement("unit");
		}
	}

	void GameData::loadWeapons(tinyxml2::XMLHandle weaponsTag) {
		// Find all weapons.
		tinyxml2::XMLElement* element = weaponsTag.FirstChildElement("weapon").ToElement();
		while (element != nullptr) {
			std::string name = convertFromText<const char*>(toText(element->FirstChildElement("name")));
			std::string image = convertFromText<const char*>(toText(element->FirstChildElement("image")));
			float damage = convertFromText<float>(toText(element->FirstChildElement("damage")));
			float timeBetweenShots = convertFromText<float>(toText(element->FirstChildElement("timeBetweenShots")));
			float range = convertFromText<float>(toText(element->FirstChildElement("range")));
			int clipSize = convertFromText<int>(toText(element->FirstChildElement("clipSize")));
			std::string shootSound = convertFromText<const char*>(toText(element->FirstChildElement("shootSound")));
			std::string reloadSound = convertFromText<const char*>(toText(element->FirstChildElement("reloadSound")));

			// Add weapon.
			weapons_[name] = new WeaponItem2D(0, 0, Weapon(damage, timeBetweenShots, range, clipSize));

			element = element->NextSiblingElement("weapon");
		}
	}

	void GameData::loadFrame(tinyxml2::XMLHandle frameTag, Animation& animation) {
		tinyxml2::XMLHandle handle = frameTag.FirstChildElement("image");
		mw::Texture texture = loadTexture(convertFromText<std::string>(toText(handle)));
		
		handle = handle.NextSiblingElement("time");
		float time = convertFromText<float>(toText(handle));
		handle = handle.NextSiblingElement("x");
		float x = convertFromText<float>(toText(handle));
		handle = handle.NextSiblingElement("y");
		float y = convertFromText<float>(toText(handle));
		handle = handle.NextSiblingElement("dx");
		float dx = convertFromText<float>(toText(handle));
		handle = handle.NextSiblingElement("dy");
		float dy = convertFromText<float>(toText(handle));
		handle = handle.NextSiblingElement("scale");
		float scale = convertFromText<float>(toText(handle));
		handle = handle.NextSiblingElement("color");
		mw::Color color;
		try {
			color = convertFromText<mw::Color>(toText(handle));
		} catch (mw::Exception&) {
			// Color tag probably missing!
			// Do nothing. Default color white.
		}

		animation.add(mw::Sprite(texture, x, y, dx, dy), scale, time);
	}

	Animation GameData::loadAnimation(tinyxml2::XMLHandle animationTag) {
		tinyxml2::XMLHandle handle = animationTag.FirstChildElement("frame");
		Animation animation;
		while (handle.ToElement() != nullptr) {
			loadFrame(handle, animation);
			handle = handle.NextSiblingElement("frame");
		}

		return animation;
	}

	bool GameData::load(tinyxml2::XMLHandle xml) {
		try {
			settings_ = loadSettings(xml.FirstChildElement("settings"));
			loadWeapons(xml.FirstChildElement("weapons"));
			loadUnits(xml.FirstChildElement("movingObjects"));
			loadCars(xml.FirstChildElement("movingObjects"));

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

	mw::Texture GameData::loadTexture(std::string file) {
		unsigned int size = textures_.size();

		mw::Texture& texture = textures_[file];
		// Image not found?
		if (textures_.size() > size) {
			texture = mw::Texture(file);

			// Image not valid?
			if (!texture.isValid()) {
				assert(0);
			}
		}

		return texture;
	}

} // Namespace zombie.
