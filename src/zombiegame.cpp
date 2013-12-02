#include "zombiegame.h"
#include "inputkeyboard.h"
#include "weapon.h"
#include "movingobject.h"
#include "building.h"
#include "auxiliary.h"
#include "load.h"
#include "settings.h"
#include "task.h"
#include "car.h"

// Graphic.
#include "mapdraw.h"
#include "buildingdraw.h"
#include "unitanimation.h"

// External.
#include <mw/exception.h>

// Stl.
#include <cmath>
#include <sstream>

namespace zombie {

	namespace {

		void drawCar(mw::Sprite& sprite, Car* car, float time) {
			State state = car->getState();
			
			glPushMatrix();
			glColor3d(1, 1, 1);
			glTranslated(state.position_.x, state.position_.y, 0);
			glRotated(state.angle_ * 180 / PI, 0, 0, 1);
			glRotated(270, 0, 0, 1);
			glScaled(car->getWidth(), car->getLength(), 1);
			sprite.draw();
			glPopMatrix();
		}
	}

	// Returns a random postion between the defined outer and inner circle centered in position.
	Position generatePosition(Position position, float innerRadius, float outerRadius) {
		return position + (innerRadius + (outerRadius - innerRadius) * random()) * Position(std::cosf(random()*2.f*3.14f), std::sinf(random()*2.f*3.14f));
	}

	ZombieGame::ZombieGame(int width, int height, tinyxml2::XMLHandle xml) : engine_(this) {
		// Set windows size.
		updateSize(width, height);

		keyboard1_ = DevicePtr(new InputKeyboard(SDLK_UP, SDLK_DOWN, SDLK_LEFT,
			SDLK_RIGHT, SDLK_SPACE, SDLK_r, SDLK_LSHIFT, SDLK_e));

		scale_ = 1.0;
		
		taskManager_.add(new MapDraw(-50, 50, -50, 50), GraphicLevel::GROUND_LEVEL);

		innerSpawnRadius_ = 10.f;
		outerSpawnRadius_ = 40.f;

		// Load game data and map data.
		load(xml);

		CarProperties volvoP = cars_["Volvo"];
		UnitProperties humanP = units_["Human"];
		UnitProperties zombieP = units_["Zombie"];

		Animation animation;
		for (auto tuple : humanP.animation_) {
			animation.add(getLoadedTexture(std::get<0>(tuple)), std::get<2>(tuple));
			animation.setScale(std::get<1>(tuple));
		}

		Position position = generatePosition(ORIGO, 0, 50);
		State state(position, ORIGO, 0);
		
		UnitAnimation* unitAnimation = new UnitAnimation(state, humanP.radius_, animation);
		taskManager_.add(unitAnimation, GraphicLevel::UNIT_LEVEL);
		auto callback = std::bind(&UnitAnimation::updateData, unitAnimation, std::placeholders::_1, std::placeholders::_2);
		
		engine_.setHuman(keyboard1_, state,
			humanP.mass_, humanP.radius_, humanP.life_, humanP.walkingSpeed_, 
			humanP.runningSpeed_, Weapon(55, 0.2f, 8, 12), callback);
		
		viewPosition_ = position;

		// Add cars.
		for (int i = 0; i < 10; ++i) {
			Animation animation;
			animation.add(getLoadedTexture(volvoP.image_));
			mw::Sprite sprite = getLoadedTexture(volvoP.image_);
			//sprite.setDrawPixelSize(true);
			Position spawn = generatePosition(ORIGO, 0, 50);
			auto callback = std::bind(drawCar, sprite, std::placeholders::_1, std::placeholders::_2);
			engine_.addCar(State(spawn, ORIGO, 0), volvoP.mass_, volvoP.life_, 
				volvoP.width_, volvoP.length_, callback);
		}

		// Add zombies.
		for (int i = 0; i < settings_.unitLevel_; ++i) {
			Animation animation;
			for (auto tuple : zombieP.animation_) {
				animation.add(getLoadedTexture(std::get<0>(tuple)), std::get<2>(tuple));
				animation.setScale(std::get<1>(tuple));
			}
			Position spawn = generatePosition(ORIGO, 0, 50);
			engine_.addAi(State(spawn, ORIGO, 0), zombieP.mass_, zombieP.radius_, 
				zombieP.life_, zombieP.walkingSpeed_, zombieP.
				runningSpeed_, true, Weapon(35, 0.5f, 1, 10000));
		}

		// Add survivors.
		for (int i = 0; i < 0; ++i) {
			Position spawn = generatePosition(ORIGO, 0, 50);
			engine_.addAi(State(spawn, ORIGO, 0), humanP.mass_, humanP.radius_, 
				humanP.life_, humanP.walkingSpeed_, humanP.runningSpeed_, 
				false, Weapon(35, 0.5, 8, 120));
		}

		for (BuildingProperties& p : buildings_) {
			engine_.addBuilding(p.points_);
			taskManager_.add(new BuildingDraw(p.points_), GraphicLevel::BUILDING_LEVEL);
		}
	}

	ZombieGame::~ZombieGame() {
	}

	// Starts the game.
	void ZombieGame::startGame() {
		engine_.start();
	}

	void ZombieGame::update(float deltaTime) {
		engine_.update(deltaTime);

		// Draw map centered around first human player.
		glPushMatrix();

		glTranslated(0.5, 0.5, 0);
		glScale2f(1.f / 50); // Is to fit the box drawn where x=[0,1] and y=[0,1].
		glScale2f(scale_); // Is to fit the box drawn where x=[0,1] and y=[0,1].
		glTranslate2f(-viewPosition_);
		
		// Game is started?
		if (engine_.isStarted()) {
			taskManager_.update(deltaTime);
		} else {
			taskManager_.update(0.0);
		}

		glPopMatrix();
	}

	void ZombieGame::humanPosition(float x, float y) {
		viewPosition_ += 0.1f* (Position(x, y) - viewPosition_);
	}

	void ZombieGame::zoom(float scale) {
		scale_ *= scale;
	}

	void ZombieGame::updateSize(int width, int height) {
		Task::width = width;
		Task::height = height;
	}

	void ZombieGame::eventUpdate(const SDL_Event& windowEvent) {
		// Update human input.
		keyboard1_->eventUpdate(windowEvent);
	}

	bool ZombieGame::load(tinyxml2::XMLHandle xml) {
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

	void ZombieGame::loadMap(std::string map) {
		try {
			// Load map file.
			tinyxml2::XMLDocument mapXml;
			mapXml.LoadFile("map.xml");
			if (mapXml.Error()) {
				// Failed!
			}

			tinyxml2::XMLHandle mapHandle(mapXml);
			mapHandle = mapHandle.FirstChildElement("map");

			auto buildings = loadBuildings(mapHandle.FirstChildElement("mapObjects"));
			for (const BuildingProperties& p : buildings) {
				buildings_.push_back(p);
			}
		} catch (std::exception&) {
			std::exit(1);
		}
	}

	mw::TexturePtr ZombieGame::getLoadedTexture(std::string file) {
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
