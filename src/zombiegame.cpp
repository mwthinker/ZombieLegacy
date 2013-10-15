#include "zombiegame.h"
#include "inputkeyboard.h"
#include "weapon.h"
#include "movingobject.h"
#include "building.h"
#include "auxiliary.h"
#include "load.h"

#include <mw/exception.h>

#include <cmath>
#include <sstream>

namespace zombie {

	// Returns a random postion between the defined outer and inner circle centered in position.
	Position generatePosition(Position position, float innerRadius, float outerRadius) {
		return position + (innerRadius + (outerRadius - innerRadius) * random()) * Position(std::cosf(random()*2.f*3.14f), std::sinf(random()*2.f*3.14f));
	}

	const Position ORIGO(0,0);

	ZombieGame::ZombieGame(int width, int height, tinyxml2::XMLHandle xml) : engine_(width, height) {
		// Set windows size.
		updateSize(width,height);
		
		keyboard1_ = DevicePtr(new InputKeyboard(SDLK_UP, SDLK_DOWN, SDLK_LEFT, 
			SDLK_RIGHT, SDLK_SPACE, SDLK_r,SDLK_LSHIFT,SDLK_e));

		engine_.addGrassGround(-50, 50, -50, 50);

		innerSpawnRadius_ = 10.f;
		outerSpawnRadius_ = 40.f;
		
		Position position = generatePosition(ORIGO, 0, 50);
		engine_.addHuman(keyboard1_,position.x, position.y, 0.3f, Weapon(55,0.2f,8,12));
		engine_.addEventListener(std::bind(&ZombieGame::handleGameEvent, this, std::placeholders::_1));

		load(xml);

		// Add cars.
		for (int i = 0; i < 10; ++i) {
			Position spawn = generatePosition(ORIGO, 0, 50);
			engine_.addCar(spawn.x, spawn.y);
		}

		// Add zombies.
		for (int i = 0; i < 1; ++i) {
			Position spawn = generatePosition(ORIGO, 0, 50);
			engine_.addAi(spawn.x, spawn.y, 0.3f, Weapon(35,0.5f,1,10000), true);
		}

		// Add survivors.
		for (int i = 0; i < 10; ++i) {
			Position spawn = generatePosition(ORIGO, 0, 50);
			engine_.addAi(spawn.x, spawn.y, 0.f, Weapon(35,0.5,8,120), false);
		}

		// Add weapons.
		for (int i = 0; i < 10; ++i) {
			Position spawn = generatePosition(ORIGO, 0, 50);
			Weapon weapon;
			engine_.addWeapon(spawn.x, spawn.y, weapon);
		}
	}

	ZombieGame::~ZombieGame() {
	}

	void ZombieGame::handleGameEvent(const GameEvent& gameEvent) {
		if (const UnitDie* unitDie = dynamic_cast<const UnitDie*>(&gameEvent)) {
			Position spawn = generatePosition(engine_.getMainUnitPostion(), innerSpawnRadius_, outerSpawnRadius_);
			engine_.addAi(spawn.x, spawn.y, 0.3f, Weapon(35,0.5f,1,10000), true);
		}
	}

	void ZombieGame::handleRemoval(bool& remove, MovingObject* mOb) {
		bool outside = mOb->getPosition().LengthSquared() > outerSpawnRadius_ * outerSpawnRadius_;
		remove = outside;
	}

	// Starts the game.
	void ZombieGame::startGame() {
		engine_.start();
	}

	void ZombieGame::update(float deltaTime) {
		engine_.update(deltaTime);
	}

	void ZombieGame::zoom(double scale) {
		engine_.zoom(scale);
	}

	void ZombieGame::updateSize(int width, int height) {
		engine_.updateSize(width, height);
	}

	void ZombieGame::eventUpdate(const SDL_Event& windowEvent) {
		// Update human input.
		keyboard1_->eventUpdate(windowEvent);
	}

	bool ZombieGame::load(tinyxml2::XMLHandle xml) {
		try {
			loadSettings(xml.FirstChildElement("settings"));
			loadWeapons(xml.FirstChildElement("weapons"));
			loadMovingUnits(xml.FirstChildElement("movingObjects"));
		} catch (mw::Exception&) {
			return false;
		}
		return true;
	}

	// Must load weapons before. Else the no weapons will be placed.
	void ZombieGame::loadMap(tinyxml2::XMLHandle xml) {
	}
	
	void ZombieGame::loadWeapons(tinyxml2::XMLHandle xml) {
		// Find all weapons.
		tinyxml2::XMLElement* element = xml.FirstChildElement("weapon").ToElement();
		while (element != nullptr) {
			tinyxml2::XMLElement* tmp = element->FirstChildElement("name")->ToElement();
			std::string name = tmp->GetText();
			
			tmp = tmp->NextSiblingElement("animation");
			std::string animation = tmp->GetText();

			tmp = tmp->NextSiblingElement("damage");
			float damage = convertFromText<float>(tmp->GetText());
			
			tmp = tmp->NextSiblingElement("timeBetweenShots");
			float timeBetweenShots = convertFromText<float>(tmp->GetText());
			
			tmp = tmp->NextSiblingElement("range");
			float range = convertFromText<float>(tmp->GetText());
			
			tmp = tmp->NextSiblingElement("clipSize");
			int clipSize = convertFromText<int>(tmp->GetText());

			tmp = tmp->NextSiblingElement("shootSound");
			std::string shootSound = tmp->GetText();

			tmp = tmp->NextSiblingElement("reloadSound");
			std::string reloadSound = tmp->GetText();

			element = element->NextSiblingElement();
		}
	}

	void ZombieGame::loadSettings(tinyxml2::XMLHandle xml) {
		try {
			tinyxml2::XMLElement* element = xml.FirstChildElement("width").ToElement();
			float w = convertFromText<float>(element->GetText());
		
			element = element->NextSiblingElement("height");
			float h = convertFromText<float>(element->GetText());

			element = element->NextSiblingElement("unitlevel");
			int unitLevel = convertFromText<int>(element->GetText());

			element = element->NextSiblingElement("zoomlevel");
			int zoomlevel = convertFromText<int>(element->GetText());
		} catch (mw::Exception&) {
			throw mw::Exception("failed to load settings in xml file");
		}
	}

	void ZombieGame::loadMovingUnits(tinyxml2::XMLHandle xml) {
		// Find all cars.
		tinyxml2::XMLElement* element = xml.FirstChildElement("car").ToElement();
		while (element != nullptr) {
			tinyxml2::XMLElement* tmp = element->FirstChildElement("name")->ToElement();
			std::string name = tmp->GetText();
			
			tmp = tmp->NextSiblingElement("animation");
			std::string animation = tmp->GetText();

			tmp = tmp->NextSiblingElement("mass");
			float mass = convertFromText<float>(tmp->GetText());
			
			tmp = tmp->NextSiblingElement("width");
			float width = convertFromText<float>(tmp->GetText());
			
			tmp = tmp->NextSiblingElement("length");
			float length = convertFromText<float>(tmp->GetText());
			
			tmp = tmp->NextSiblingElement("life");
			float life = convertFromText<float>(tmp->GetText());

			element = element->NextSiblingElement("car");
		}

		// Find all units.
		element = xml.FirstChildElement("unit").ToElement();
		while (element != nullptr) {
			tinyxml2::XMLElement* tmp = element->FirstChildElement("name")->ToElement();
			std::string name = tmp->GetText();

			tmp = tmp->NextSiblingElement("mass");
			float mass = convertFromText<float>(tmp->GetText());
			
			tmp = tmp->NextSiblingElement("radius");
			float radius = convertFromText<float>(tmp->GetText());
			
			tmp = tmp->NextSiblingElement("life");
			float life = convertFromText<float>(tmp->GetText());
			
			tmp = tmp->NextSiblingElement("infected");
			bool infected = convertFromText<bool>(tmp->GetText());
			
			tmp = tmp->NextSiblingElement("walkingSpeed");
			float walkingSpeed = convertFromText<float>(tmp->GetText());

			tmp = tmp->NextSiblingElement("runningSpeed");
			float runningSpeed = convertFromText<float>(tmp->GetText());

			tmp = tmp->NextSiblingElement("stamina");
			float stamina = convertFromText<float>(tmp->GetText());

			element = element->NextSiblingElement("unit");
		}
	}

} // Namespace zombie.
