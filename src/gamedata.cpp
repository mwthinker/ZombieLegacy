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
		Animation animation = loadAnimation(carTag.FirstChildElement("moveAnimation"));
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
		Animation moveA = loadAnimation(humanTag.FirstChildElement("moveAnimation"));
		Animation injuredA = loadAnimation(humanTag.FirstChildElement("injuredAnimation"));
		Animation dieA = loadAnimation(humanTag.FirstChildElement("dieAnimation"));
		mw::Sound dieS = loadSound(zombie::extract<std::string>(humanTag.FirstChildElement("dieSound")));
		mw::Sound hitS = loadSound(zombie::extract<std::string>(humanTag.FirstChildElement("hitSound")));
		std::string weaponName = zombie::extract<std::string>(humanTag.FirstChildElement("weapon"));
		
		dataInterface.loadHuman(mass, radius, life, walkingSpeed, runningSpeed, stamina, moveA, injuredA, dieA, dieS, hitS, weaponName);
	}

	void GameData::loadExplosion(DataInterface& dataInterface) const {
		tinyxml2::XMLConstHandle explosionTag = tinyxml2::XMLConstHandle(xmlDoc_.FirstChildElement("zombieGame")).FirstChildElement("explosion");
		mw::Texture particle = loadTexture(zombie::extract<std::string>(explosionTag.FirstChildElement("particleImage")));
		mw::Sprite shockwave;
		extract(shockwave, explosionTag.FirstChildElement("shockwaveImage"));
		mw::Sprite emitter;
		extract(emitter, explosionTag.FirstChildElement("emitterImage"));
		mw::Sound sound = loadSound(zombie::extract<std::string>(explosionTag.FirstChildElement("shockwaveImage")));
		dataInterface.loadExplosion(particle, shockwave, emitter , sound);
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
		Animation moveA = loadAnimation(zombieTag.FirstChildElement("moveAnimation"));
		Animation injuredA = loadAnimation(zombieTag.FirstChildElement("injuredAnimation"));
		Animation dieA = loadAnimation(zombieTag.FirstChildElement("dieAnimation"));
		mw::Sound dieS = loadSound(zombie::extract<std::string>(zombieTag.FirstChildElement("dieSound")));
		mw::Sound hitS = loadSound(zombie::extract<std::string>(zombieTag.FirstChildElement("hitSound")));
		std::string weaponName = zombie::extract<std::string>(zombieTag.FirstChildElement("weapon"));

		dataInterface.loadZombie(mass, radius, life, walkingSpeed, runningSpeed, stamina, moveA, injuredA, dieA, dieS, hitS, weaponName);
	}

	void GameData::loadMap(DataInterface& dataInterface) const {
		tinyxml2::XMLConstHandle handleXml = tinyxml2::XMLConstHandle(xmlMap_.FirstChildElement("map")).FirstChildElement("objects").FirstChildElement("object");
		while (handleXml.ToElement() != nullptr) {
			if (handleXml.ToElement()->Attribute("type", "building")) {
				std::string geom = zombie::extract<std::string>(handleXml.FirstChildElement("geom"));
				std::stringstream stream(geom);
				std::string word;

				if (stream >> word) {
					if (word == "POLYGON") {
						dataInterface.loadBuilding(loadPolygon(stream.str()));
					}
				}
			} else if (handleXml.ToElement()->Attribute("type", "water")) {
				std::string geom = zombie::extract<std::string>(handleXml.FirstChildElement("geom"));
				std::stringstream stream(geom);
				std::string word;

				if (stream >> word) {
					if (word == "POLYGON") {
						dataInterface.loadWater(loadPolygon(stream.str()));
					}
				}
			} else if (handleXml.ToElement()->Attribute("type", "road")) {
				std::string geom = zombie::extract<std::string>(handleXml.FirstChildElement("geom"));
				std::stringstream stream(geom);
				std::string word;

				if (stream >> word) {
					if (word == "POLYGON") {
						dataInterface.loadRoad(loadPolygon(stream.str()));
					}
				}
			}

			handleXml = handleXml.NextSiblingElement("object");
		}
	}

	void GameData::loadWeapon(DataInterface& dataInterface) const {
		// Find all weapons.
		tinyxml2::XMLConstHandle weaponTag = tinyxml2::XMLConstHandle(xmlDoc_.FirstChildElement("zombieGame")).FirstChildElement("weapons").FirstChildElement("weapon");
		while (weaponTag.ToElement() != nullptr) {
			std::string name = zombie::extract<std::string>(weaponTag.FirstChildElement("name"));
			mw::Sprite sprite;
			extract(sprite, weaponTag.FirstChildElement("image"));

			float damage = zombie::extract<float>(weaponTag.FirstChildElement("damage"));
			float timeBetweenShots = zombie::extract<float>(weaponTag.FirstChildElement("timeBetweenShots"));
			float range = zombie::extract<float>(weaponTag.FirstChildElement("range"));
			int clipSize = zombie::extract<int>(weaponTag.FirstChildElement("clipSize"));
			
			mw::Sound shoot = loadSound(zombie::extract<std::string>(weaponTag.FirstChildElement("shootSound")));
			mw::Sound reload = loadSound(zombie::extract<std::string>(weaponTag.FirstChildElement("reloadSound")));
			Animation animation = loadAnimation(weaponTag.FirstChildElement("animation"));

			dataInterface.loadWeapon(name, damage, timeBetweenShots, range, clipSize, sprite, animation, shoot, reload);
			
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

	mw::Sprite GameData::getMenuBackgroundImage() const {
		tinyxml2::XMLConstHandle handleXml = tinyxml2::XMLConstHandle(xmlDoc_.FirstChildElement("zombieGame")).FirstChildElement("menu");
		return loadTexture(zombie::extract<std::string>(handleXml.FirstChildElement("backGroundImage")));
	}

	std::vector<Position> GameData::loadSpawningPoints() const {
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

	void GameData::loadFrame(tinyxml2::XMLConstHandle frameTag, Animation& animation) const {
		mw::Sprite sprite;
		GameData::extract(sprite, frameTag.FirstChildElement("image"));

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

	void GameData::extract(mw::Sprite& sprite, tinyxml2::XMLConstHandle handle) const {
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

		sprite = mw::Sprite(texture, x, y, w, h);
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
