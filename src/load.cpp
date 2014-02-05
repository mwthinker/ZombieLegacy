#include "load.h"

#include <tinyxml2.h>

#include <vector>
#include <string>

namespace zombie {

	namespace {

		std::stringstream& operator>>(std::stringstream& stream, Point& point) {
			stream >> point.x;
			stream >> point.y;
			return stream;
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
			while (stream >> point) {
				points.push_back(point);
			}
			points.pop_back();
			return points;
		}

		// Todo!
		Point loadPoint(std::string line);

		struct Circle {
			Point centre_;
			float radius_;
		};

		// Todo!
		Circle loadCircle(std::string line);

		std::vector<TupleImageScaleTime> loadAnimation(tinyxml2::XMLHandle xml) {
			tinyxml2::XMLHandle handle = xml.FirstChildElement("scale");
			if (handle.ToElement() == nullptr) {
				throw  std::exception();
			}
			float scale = convertFromText<float>(handle.ToElement()->GetText());
			std::vector<TupleImageScaleTime> animation;

			while (handle.NextSiblingElement("image").ToElement() != nullptr) {
				handle = handle.NextSiblingElement("image");
				std::string image = handle.ToElement()->GetText();
				handle = handle.NextSiblingElement("time");
				float time = convertFromText<float>(handle.ToElement()->GetText());
				animation.push_back(TupleImageScaleTime(image, scale, time));
			}

			return move(animation);
		}

		// Returns the value of the tag's element. If the value is empty, a empty
		// string is returned. If the element don't exist a runtime exception is thrown.
		inline const char* toText(tinyxml2::XMLHandle handle) {
			tinyxml2::XMLElement* element = handle.ToElement();
			if (element == nullptr) {
				throw mw::Exception("convertFromText failed");
			}
			return element->GetText();
		}

		// Returns the tag's element. If the element don't exist a runtime exception is thrown.
		// The return value is never null.
		inline tinyxml2::XMLElement* toElement(tinyxml2::XMLHandle handle) {
			tinyxml2::XMLElement* element = handle.ToElement();
			if (element == nullptr) {
				throw mw::Exception("convertFromText failed");
			}
			return element;
		}

	} // Anonymous namespace.

	template <>
	const char* convertFromText<const char*>(const char* txt) {
		return txt;
	}
		
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

	std::vector<BuildingProperties> loadBuildings(tinyxml2::XMLHandle objectsTag) {
		std::vector<BuildingProperties> buildings;

		tinyxml2::XMLElement* element = objectsTag.FirstChildElement("object").ToElement();
		while (element != nullptr) {
			if (element->Attribute("type", "building")) {
				std::string geom = convertFromText<const char*>(toText(element->FirstChildElement("geom")));
				std::stringstream stream(geom);
				std::string word;

				if (stream >> word) {
					if (word == "POLYGON") {
						BuildingProperties properties;
						properties.points_ = loadPolygon(stream.str());
						buildings.push_back(properties);
					}
				}
			}

			element = element->NextSiblingElement("object");
		}

		return buildings;
	}

	Terrain2D loadTerrain(tinyxml2::XMLHandle objectsTag) {
		Terrain2D terrain;

		tinyxml2::XMLElement* element = objectsTag.FirstChildElement("object").ToElement();
		while (element != nullptr) {
			if(element->Attribute("type", "road")) {
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
		}
		

		return terrain;
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

} // Namespace zombie.
