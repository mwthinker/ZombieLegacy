#include "gamedata.h"
#include "building2d.h"
#include "car2d.h"
#include "unit2d.h"
#include "weaponitem2d.h"
#include "weapon2d.h"

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

		std::stringstream& operator>>(std::stringstream& stream, Point& point) {
			stream >> point.x;
			stream >> point.y;
			return stream;
		}

		// Override the stream operator for a color defined as a string "(r,g,b)".
        // Any space character will be ignored but r, g, b represent a float value
        // and must be seperated by "," and the whole string surrounded by "(" and ")".
        // Example of correct usaged:
        // stringstream stream;
        // stream << "( 1.0, 0.1,2);
        // Color color;
        // stream >> color;
        std::stringstream& operator>>(std::stringstream& stream, mw::Color& color) {
            bool start = false;
            bool end = false;

            std::string str;
            std::string tmp;
            while (stream >> tmp) {
                if (tmp.size() > 0 && !start) {
                    if (tmp[0] == '(') {
                        start = true;
                        tmp[0] = ' ';
                    } else {
                        // Failed.
                        stream.setstate(std::ios::failbit);
                        return stream;
                    }
                }
                if (tmp.size() > 0 && !end) {
                    for (char& c : tmp) {
                        if (c == ')') {
                            c = ' ';
                            end = true;
                            break;
                        }
                    }
                }
                str += tmp;
                if (start && end) {
                    break;
                }
            }

            if (!start || !end) {
                // Failed.
                stream.setstate(std::ios::failbit);
                return stream;
            }

            std::replace(str.begin(), str.end(), ',', ' ');
            std::stringstream newStream_(str);
            newStream_ >> color.red_;
            newStream_ >> color.green_;
            newStream_ >> color.blue_;
            float alpha = 1;
            if (newStream_ >> alpha) {
                color.alpha_ = alpha;
            }

            return stream;
        }

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

        template <class Output>
        Output extract(tinyxml2::XMLHandle handle) {
            tinyxml2::XMLElement* element = handle.ToElement();
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

        template <class Output>
        void extract(Output& value, tinyxml2::XMLHandle handle) {
            value = extract<Output>(handle);
        }

        template <class Input>
        void insert(const Input& input, tinyxml2::XMLHandle handle) {
            tinyxml2::XMLElement* element = handle.ToElement();
            if (element == nullptr) {
                throw mw::Exception("Missing element!");
            }

            std::stringstream stream;
            stream << input;
            if (stream.fail()) {
                throw mw::Exception("Stream failed!");
            }

            element->SetText(stream.str().c_str());
        }

		// Takes c-string as input and returns the correct conversion.
		// Throws mw::Exception if the input is null or if the conversion
		// fails.
		// E.g. float a = convertFromText<float>("1.2");
		// Gives output, a = 1.2;
		template <class Output>
		Output convertFromText(const char* txt) {
			if (txt == nullptr) {
				throw mw::Exception("convertFromText, input null");
			}

			std::stringstream stream(txt);
			Output output;
			stream >> output;
			return output;
		}

		Point loadPoint(std::string line) {
			int index = line.find("(");
			int index2 = line.rfind(")");

			// New line is the string between "((" and "))".
			line = line.substr(index + 1, index2 - index - 1);

			// Replace all ',' (comma) with whitespace.
			for (char& chr : line) {
				if (chr == ',') {
					chr = ' ';
				}
			}

			return convertFromText<Point>(line.c_str());
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

		template <>
		const char* convertFromText<const char*>(const char* txt) {
			if (txt == nullptr) {
				throw mw::Exception("convertFromText, input null");
			}
			if (txt[0] == '\n') {
				throw mw::Exception("convertFromText, string is empty");
			}
			return txt;
		}

		// Returns the value of the tag's element. If the value is empty, a empty
		// string is returned. If the element don't exist a runtime exception is thrown.
		const char* toText(tinyxml2::XMLHandle handle) {
			tinyxml2::XMLElement* element = handle.ToElement();
			if (element == nullptr) {
				throw mw::Exception("convertFromText failed");
			}
			return element->GetText();
		}

		// Returns the tag's element. If the element don't exist a runtime exception is thrown.
		// The return value is never null.
		tinyxml2::XMLElement* toElement(tinyxml2::XMLHandle handle) {
			tinyxml2::XMLElement* element = handle.ToElement();
			if (element == nullptr) {
				throw mw::Exception("convertFromText failed");
			}
			return element;
		}

		void loadMapItems(tinyxml2::XMLHandle objectsTag, std::vector<Building2D*>& buildings, std::vector<Position>& spawningPoints, Terrain2D& terrain) {
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
				} else if (element->Attribute("type", "spawningpoint")) {
					std::string geom = convertFromText<const char*>(toText(element->FirstChildElement("geom")));
					std::stringstream stream(geom);
					std::string word;

					if (stream >> word) {
						if (word == "POINT") {
							spawningPoints.push_back(loadPoint(stream.str()));
						}
					}
				}

				element = element->NextSiblingElement("object");
			}
		}

		Settings loadSettings(tinyxml2::XMLHandle settingsTag) {
			Settings settings;
			settings.defaultFont_ = convertFromText<std::string>(toText(settingsTag.FirstChildElement("font")));
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

		// Load game data and map data.
		load(handleXml);
		dataFile_ = dataFile;
	}

	void GameData::save() {
        xmlDoc_.SaveFile(dataFile_.c_str());
    }

    void GameData::setWindowSize(int width, int height) {
        tinyxml2::XMLHandle handleXml = tinyxml2::XMLHandle(xmlDoc_.FirstChildElement("zombie")).FirstChildElement("settings");
        zombie::insert(width, handleXml.FirstChildElement("width"));
        zombie::insert(height, handleXml.FirstChildElement("height"));
        save();
    }

    int GameData::getWindowWidth() const {
        tinyxml2::XMLConstHandle handlemXl = tinyxml2::XMLConstHandle(xmlDoc_.FirstChildElement("zombie")).FirstChildElement("settings");
        return zombie::extract<int>(handlemXl.FirstChildElement("width"));
    }
    int GameData::getWindowHeight() const {
        const tinyxml2::XMLConstHandle handleXml = tinyxml2::XMLConstHandle(xmlDoc_.FirstChildElement("zombie")).FirstChildElement("settings");
        return zombie::extract<int>(handleXml.FirstChildElement("height"));
    }

    void GameData::setWindowPosition(int x, int y) {
        tinyxml2::XMLHandle handleXml = tinyxml2::XMLHandle(xmlDoc_.FirstChildElement("zombie")).FirstChildElement("settings");
        zombie::insert(x, handleXml.FirstChildElement("positionX"));
        zombie::insert(y, handleXml.FirstChildElement("positionY"));
        save();
    }

    int GameData::getWindowXPosition() const {
        tinyxml2::XMLConstHandle handleXml = tinyxml2::XMLConstHandle(xmlDoc_.FirstChildElement("zombie")).FirstChildElement("settings");
        return zombie::extract<int>(handleXml.FirstChildElement("positionX"));
    }

    int GameData::getWindowYPosition() const {
        tinyxml2::XMLConstHandle handleXml = tinyxml2::XMLConstHandle(xmlDoc_.FirstChildElement("zombie")).FirstChildElement("settings");
        return zombie::extract<int>(handleXml.FirstChildElement("positionY"));
    }

    void GameData::setWindowMaximized(bool maximized) {
        tinyxml2::XMLHandle handleXml = tinyxml2::XMLHandle(xmlDoc_.FirstChildElement("zombie")).FirstChildElement("settings");
        zombie::insert(maximized, handleXml.FirstChildElement("maximized"));
        save();
    }

    bool GameData::isWindowMaximized() const {
        tinyxml2::XMLConstHandle handleXml = tinyxml2::XMLConstHandle(xmlDoc_.FirstChildElement("zombie")).FirstChildElement("settings");
        return zombie::extract<bool>(handleXml.FirstChildElement("maximized"));
    }

    bool GameData::loadCar(std::string name, const Car2D& car) {
        tinyxml2::XMLHandle handleXml = tinyxml2::XMLHandle(xmlDoc_.FirstChildElement("zombie")).FirstChildElement("Cars").FirstChildElement(name.c_str());

        //return zombie::extract<bool>(handleXml.FirstChildElement("maximized"));
    }

    bool GameData::loadUnit(std::string name, const Unit2D& unit) {
    }

    std::vector<Building2D*> GameData::loadBuildings() {

    }

    float GameData::getImpulseThreshold() const {
    }

    int GameData::getTimeStemMS() const {
    }

    Terrain2D GameData::getTerrain2D() const {
    }

    int GameData::getUnitLevel() const {
    }

	void GameData::loadCars(tinyxml2::XMLHandle movingUnitsTag) {
		// Find all cars.
		tinyxml2::XMLElement* element = movingUnitsTag.FirstChildElement("car").ToElement();
		while (element != nullptr) {
			std::string name = convertFromText<const char*>(toText(element->FirstChildElement("name")));
			Animation animation = loadAnimation(toElement(element->FirstChildElement("animation")));

			float mass = convertFromText<float>(toText(element->FirstChildElement("mass")));
			float width = convertFromText<float>(toText(element->FirstChildElement("width")));
			float length = convertFromText<float>(toText(element->FirstChildElement("length")));
			float life = convertFromText<float>(toText(element->FirstChildElement("life")));

			//cars_[name] = new Car2D(mass, life, width, length, animation);

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
			//units_[name] = new Unit2D(mass, radius, life, walkingSpeed, runningSpeed, infected, weapons_[weaponName], animation);

			element = element->NextSiblingElement("unit");
		}
	}

	void GameData::loadWeapons(tinyxml2::XMLHandle weaponsTag) {
		// Find all weapons.
		tinyxml2::XMLElement* element = weaponsTag.FirstChildElement("weapon").ToElement();
		while (element != nullptr) {
			std::string name = convertFromText<const char*>(toText(element->FirstChildElement("name")));
			std::string image = convertFromText<const char*>(toText(element->FirstChildElement("image")));
			mw::Texture texture = loadTexture(toText(element->FirstChildElement("image")));

			float damage = convertFromText<float>(toText(element->FirstChildElement("damage")));
			float timeBetweenShots = convertFromText<float>(toText(element->FirstChildElement("timeBetweenShots")));
			float range = convertFromText<float>(toText(element->FirstChildElement("range")));
			int clipSize = convertFromText<int>(toText(element->FirstChildElement("clipSize")));
			std::string shootSound = convertFromText<const char*>(toText(element->FirstChildElement("shootSound")));
			std::string reloadSound = convertFromText<const char*>(toText(element->FirstChildElement("reloadSound")));
			Animation animation = loadAnimation(toElement(element->FirstChildElement("animation")));

			// Add weapon.
			weapons_[name] = std::make_shared<Weapon2D>(damage, timeBetweenShots, range, clipSize, texture, animation);

			element = element->NextSiblingElement("weapon");
		}
	}

	void GameData::loadFrame(tinyxml2::XMLHandle frameTag, Animation& animation) {
		tinyxml2::XMLHandle handle = frameTag.FirstChildElement("image");
		mw::Texture texture = loadTexture(convertFromText<std::string>(toText(handle)));

		float time = 1;
		tinyxml2::XMLHandle tmp = handle.NextSiblingElement("time");
		try {
			time = convertFromText<float>(toText(tmp));
			handle = tmp;
		} catch (mw::Exception&) {
			// Time tag probably missing!
			// Do nothing. Default time 1.
		}

		handle = handle.NextSiblingElement("x");
		float x = convertFromText<float>(toText(handle));
		handle = handle.NextSiblingElement("y");
		float y = convertFromText<float>(toText(handle));
		handle = handle.NextSiblingElement("dx");
		float dx = convertFromText<float>(toText(handle));
		handle = handle.NextSiblingElement("dy");
		float dy = convertFromText<float>(toText(handle));
		handle = handle.NextSiblingElement("bodyWidth");
		float bodyWidth = convertFromText<float>(toText(handle));

		animation.add(mw::Sprite(texture, x, y, dx, dy), bodyWidth, time);
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
			loadWeapons(xml.FirstChildElement("weapons"));
			loadUnits(xml.FirstChildElement("movingObjects"));
			loadCars(xml.FirstChildElement("movingObjects"));

			// Load map.
			//loadMap(settings_.mapFile_);

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

			//loadMapItems(mapHandle.FirstChildElement("objects"), buildings_, spawningPoints_, terrain2d_);

		} catch (std::exception&) {
			std::exit(1);
		}
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

    mw::Texture GameData::loadTexture(std::string file) {
        unsigned int size = textures_.size();
        mw::Texture& texture = textures_[file];

        // Image not found?
        if (textures_.size() > size) {
            texture = mw::Texture(file);

            // Image not valid?
            if (!texture.isValid()) {
                std::cerr << file << " failed to load!" << std::endl;
            }
        }

        return texture;
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

} // Namespace zombie.
