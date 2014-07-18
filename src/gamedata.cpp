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

		// Specialization for string type. In order to insure that the whole text is 
		// returned as a string.
		template <>
		std::string extract(tinyxml2::XMLHandle handle) {
			tinyxml2::XMLElement* element = handle.ToElement();
			if (element == nullptr) {
				throw mw::Exception("Missing element!");
			}
			const char* str = element->GetText();

			if (str == nullptr) {
				throw mw::Exception("Missing text!");
			}

			return std::string(str);
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

			std::stringstream stream(line);
			Point point;
			stream >> point;
			return point;
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

	}

	GameData::GameData(std::string dataFile) : dataFile_(dataFile) {
		// Load XML file.
		xmlDoc_.LoadFile(dataFile.c_str());
		if (xmlDoc_.Error()) {
			// Failed!
			xmlDoc_.PrintError();
		} else {
			try {
				font_ = zombie::extract<std::string>(tinyxml2::XMLHandle(xmlDoc_.FirstChildElement("zombie")).FirstChildElement("settings").FirstChildElement("font"));

				loadWeapons();

				// Load map.
				xmlMap_.LoadFile("map.xml");
				if (xmlMap_.Error()) {
					xmlDoc_.PrintError();
				}
			} catch (mw::Exception&) {
				int a = 1;
			}
		}
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

	bool GameData::loadCar(std::string carName, Car2D& car) const {
		tinyxml2::XMLConstHandle handleXml = tinyxml2::XMLConstHandle(xmlDoc_.FirstChildElement("zombie")).FirstChildElement("movingObjects").FirstChildElement("car");
		try {
			while (handleXml.ToElement() != nullptr) {
				std::string name = zombie::extract<std::string>(handleXml.FirstChildElement("name"));
				if (name == carName) {
					Animation animation = loadAnimation(handleXml.FirstChildElement("animation"));
					float mass = zombie::extract<float>(handleXml.FirstChildElement("mass"));
					float width = zombie::extract<float>(handleXml.FirstChildElement("width"));
					float length = zombie::extract<float>(handleXml.FirstChildElement("length"));
					float life = zombie::extract<float>(handleXml.FirstChildElement("life"));
					car = Car2D(mass, life, width, length, animation);
					return true;
				}
				handleXml = handleXml.NextSiblingElement();
			}
		} catch (mw::Exception&) {
			return false;
		}
		return false;
    }

	bool GameData::loadUnit(std::string unitName, Unit2D& unit) const {
		tinyxml2::XMLConstHandle handleXml = tinyxml2::XMLConstHandle(xmlDoc_.FirstChildElement("zombie")).FirstChildElement("movingObjects").FirstChildElement("unit");
		try {
			while (handleXml.ToElement() != nullptr) {
				std::string name = zombie::extract<std::string>(handleXml.FirstChildElement("name"));
				if (name == unitName) {
					float mass = zombie::extract<float>(handleXml.FirstChildElement("mass"));
					float radius = zombie::extract<float>(handleXml.FirstChildElement("radius"));
					float life = zombie::extract<float>(handleXml.FirstChildElement("life"));
					bool infected = zombie::extract<bool>(handleXml.FirstChildElement("infected"));
					float walkingSpeed = zombie::extract<float>(handleXml.FirstChildElement("walkingSpeed"));
					float runningSpeed = zombie::extract<float>(handleXml.FirstChildElement("runningSpeed"));
					float stamina = zombie::extract<float>(handleXml.FirstChildElement("stamina"));
					Animation animation = loadAnimation(handleXml.FirstChildElement("animation"));
					std::string weaponName = zombie::extract<std::string>(handleXml.FirstChildElement("weapon"));
					unit = Unit2D(mass, radius, life, walkingSpeed, runningSpeed, infected, weapons_[weaponName], animation);
					return true;
				}
				handleXml = handleXml.NextSiblingElement("unit");
			}
		} catch (mw::Exception&) {
			return false;
		}
		return false;
    }

	bool GameData::loadBuildings(std::vector<Building2D>& buildings) const {
		tinyxml2::XMLConstHandle handleXml = tinyxml2::XMLConstHandle(xmlMap_.FirstChildElement("map")).FirstChildElement("objects").FirstChildElement("object");
		try {
			while (handleXml.ToElement() != nullptr) {
				if (handleXml.ToElement()->Attribute("type", "building")) {
					std::string geom = zombie::extract<std::string>(handleXml.FirstChildElement("geom"));
					std::stringstream stream(geom);
					std::string word;

					if (stream >> word) {
						if (word == "POLYGON") {
							buildings.emplace_back(loadPolygon(stream.str()));
						}
					}
				}
				handleXml = handleXml.NextSiblingElement("object");
			}
			return true;
		} catch (mw::Exception&) {
			return false;
		}
		return false;
    }

    float GameData::getImpulseThreshold() const {
		tinyxml2::XMLConstHandle handleXml = tinyxml2::XMLConstHandle(xmlDoc_.FirstChildElement("zombie")).FirstChildElement("settings");
		return zombie::extract<float>(handleXml.FirstChildElement("impulseThreshold"));
    }

    int GameData::getTimeStemMS() const {
		tinyxml2::XMLConstHandle handleXml = tinyxml2::XMLConstHandle(xmlDoc_.FirstChildElement("zombie")).FirstChildElement("settings");
		return zombie::extract<int>(handleXml.FirstChildElement("timeStepMS"));
    }

    Terrain2D GameData::getTerrain2D() const {
		tinyxml2::XMLConstHandle handleXml = tinyxml2::XMLConstHandle(xmlDoc_.FirstChildElement("zombie")).FirstChildElement("weapons").FirstChildElement("weapon");
		Terrain2D terrain2d;
		while (handleXml.ToElement() != nullptr) {
			if (handleXml.ToElement()->Attribute("type", "water")) {
				std::string geom = zombie::extract<std::string>(handleXml.FirstChildElement("geom"));
				std::stringstream stream(geom);
				std::string word;

				if (stream >> word) {
					if (word == "POLYGON") {
						terrain2d.addWater(loadPolygon(stream.str()));
					}
				}
			} else if (handleXml.ToElement()->Attribute("type", "road")) {
				std::string geom = zombie::extract<std::string>(handleXml.FirstChildElement("geom"));
				std::stringstream stream(geom);
				std::string word;

				if (stream >> word) {
					if (word == "POLYGON") {
						terrain2d.addRoad(loadPolygon(stream.str()));
					}
				}
			} else if (handleXml.ToElement()->Attribute("type", "roadline")) {
				std::string geom = zombie::extract<std::string>(handleXml.FirstChildElement("geom"));
				std::stringstream stream(geom);
				std::string word;

				if (stream >> word) {
					if (word == "POLYGON") {
						terrain2d.addRoadLine(loadPolygon(stream.str()));
					}
				}
			}

			handleXml = handleXml.NextSiblingElement("object");
		}
		return terrain2d;
    }

    int GameData::getUnitLevel() const {
		tinyxml2::XMLConstHandle handleXml = tinyxml2::XMLConstHandle(xmlDoc_.FirstChildElement("zombie")).FirstChildElement("settings");
		return zombie::extract<int>(handleXml.FirstChildElement("unitLevel"));
    }

	std::vector<Position> GameData::getSpawningPoints() const {
		tinyxml2::XMLConstHandle handleXml = tinyxml2::XMLConstHandle(xmlMap_.FirstChildElement("map")).FirstChildElement("objects").FirstChildElement("object");
		std::vector<Position> points_;
		while (handleXml.ToElement() != nullptr) {
			if (handleXml.ToElement()->Attribute("type", "spawningpoint")) {
				std::string geom = zombie::extract<std::string>(handleXml.FirstChildElement("geom"));
				std::stringstream stream(geom);
				std::string word;

				if (stream >> word) {
					if (word == "POINT") {
						points_.push_back(loadPoint(stream.str()));
					}
				}
			}

			handleXml = handleXml.NextSiblingElement("object");
		}
		return points_;
	}

	void GameData::loadWeapons() const {
		// Find all weapons.
		tinyxml2::XMLConstHandle handleXml = tinyxml2::XMLConstHandle(xmlDoc_.FirstChildElement("zombie")).FirstChildElement("weapons").FirstChildElement("weapon");
		while (handleXml.ToElement() != nullptr) {
			std::string name = zombie::extract<std::string>(handleXml.FirstChildElement("name"));
			std::string image = zombie::extract<std::string>(handleXml.FirstChildElement("image"));
			mw::Texture texture = loadTexture(zombie::extract<std::string>(handleXml.FirstChildElement("image")));

			float damage = zombie::extract<float>(handleXml.FirstChildElement("damage"));
			float timeBetweenShots = zombie::extract<float>(handleXml.FirstChildElement("timeBetweenShots"));
			float range = zombie::extract<float>(handleXml.FirstChildElement("range"));
			int clipSize = zombie::extract<int>(handleXml.FirstChildElement("clipSize"));
			std::string shootSound = zombie::extract<std::string>(handleXml.FirstChildElement("shootSound"));
			std::string reloadSound = zombie::extract<std::string>(handleXml.FirstChildElement("reloadSound"));
			Animation animation = loadAnimation(handleXml.FirstChildElement("animation"));

			// Add weapon.
			weapons_[name] = std::make_shared<Weapon2D>(damage, timeBetweenShots, range, clipSize, texture, animation);
			handleXml = handleXml.NextSiblingElement("weapon");
		}
	}

	void GameData::loadFrame(tinyxml2::XMLConstHandle frameTag, Animation& animation) const {
		mw::Texture texture = loadTexture(zombie::extract<std::string>(frameTag.FirstChildElement("image")));

		float time = 1;
		try {
			time = zombie::extract<float>(frameTag.FirstChildElement("time"));
		} catch (mw::Exception&) {
			// Time tag probably missing!
			// Do nothing. Default time 1.
		}
		float x = zombie::extract<float>(frameTag.FirstChildElement("x"));
		float y = zombie::extract<float>(frameTag.FirstChildElement("y"));
		float dx = zombie::extract<float>(frameTag.FirstChildElement("dx"));
		float dy = zombie::extract<float>(frameTag.FirstChildElement("dy"));
		float bodyWidth = zombie::extract<float>(frameTag.FirstChildElement("bodyWidth"));

		animation.add(mw::Sprite(texture, x, y, dx, dy), bodyWidth, time);
	}

	Animation GameData::loadAnimation(tinyxml2::XMLConstHandle animationTag) const {
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
                std::cerr << file << " failed to load!" << std::endl;
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
