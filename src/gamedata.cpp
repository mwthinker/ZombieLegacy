#include "gamedata.h"
#include "load.h"

#include <mw/sound.h>
#include <mw/texture.h>
#include <mw/exception.h>

#include <tinyxml2.h>

#include <map>
#include <string>

namespace zombie {

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

	void GameData::iterateBuildings(std::function<void(BuildingProperties)> func) {
		for (BuildingProperties& p : buildings_) {
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

			auto buildings = loadBuildings(mapHandle.FirstChildElement("objects"));
			for (const BuildingProperties& p : buildings) {
				buildings_.push_back(p);
			}
			mapHandle = mapHandle.FirstChildElement("map");
			terrain2d_ = loadTerrain(mapHandle.FirstChildElement("objects"));
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
