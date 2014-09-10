#include "zombiegame.h"
#include "auxiliary.h"
#include "inputkeyboard.h"
#include "unit2d.h"
#include "car2d.h"
#include "weapon2d.h"
#include "weaponitem2d.h"
#include "building2d.h"
#include "terrain2d.h"
#include "shot.h"
#include "graphicanimation.h"
#include "explosion.h"
#include "fog.h"
#include "tree2D.h"
#include "gun.h"
#include "missilelauncher2d.h"
#include "gamedataentry.h"

#include <mw/exception.h>

#include <cmath>
#include <sstream>

namespace zombie {

	namespace {

		// Returns a random postion between the defined outer and inner circle centered in position.
		Position generatePosition(Position position, float innerRadius, float outerRadius) {
			float angle = random(0, 2.f*PI);
			return position + (innerRadius + (outerRadius - innerRadius) * random()) * Position(std::cos(angle), std::sin(angle));
		}

		Position generatePosition(std::vector<Position> positions) {
			return positions[randomInt(0, positions.size() - 1)];
		}

		void removeDeadGraphicObjects(std::list<std::shared_ptr<Graphic>>& list) {
			list.remove_if([](const std::shared_ptr<Graphic>& ob) {
				return ob->toBeRemoved();
			});
		}

		void drawGraphicList(std::list<std::shared_ptr<Graphic>>& list, float deltaTime, gui::WindowMatrixPtr wPtr) {
			for (auto ob : list) {
				ob->draw(deltaTime, wPtr);
			}
		}

		Missile2D loadMissile2D(GameInterface* gameInterface, GameDataEntry& entry, float damage, float range) {
			float mass = entry.getFloat("range");
			float width = entry.getFloat("width");
			float length = entry.getFloat("length");
			Animation animation = entry.getAnimation("animation");
			mw::Sound moveSound = entry.getSound("moveSound");
			float damageRadius = entry.getFloat("damageRadius");
			float deathTime = entry.getFloat("deathTime");
			float speed = entry.getFloat("speed");

			return Missile2D(animation, *gameInterface, width, length, mass, speed, deathTime, damage, damageRadius);
		}

		Weapon2D loadWeapon2D(GameInterface* gameInterface, GameDataEntry& entry) {
			mw::Sprite symbolImage = entry.getSprite("symbolImage");

			float timeBetweenShots = entry.getFloat("timeBetweenShots");
			int clipSize = entry.getInt("clipSize");

			mw::Sound shoot = entry.getSound("shootSound");
			mw::Sound reload = entry.getSound("reloadSound");
			Animation animation = entry.getAnimation("moveAnimation");
			float size = entry.getFloat("size");
			Position grip;
			grip.x = entry.getFloat("moveImageGripX");
			grip.y = entry.getFloat("moveImageGripY");
			
			GameDataEntry projectile = entry.getChildEntry("projectile");
			float damage = projectile.getFloat("damage");
			float range = projectile.getFloat("range");

			if (projectile.isAttributeEqual("type", "missile")) {
				auto missile = loadMissile2D(gameInterface, projectile, damage, range);
				auto missileLauncher = std::make_shared<MissileLauncher2D>(missile, clipSize, timeBetweenShots, range, shoot, reload);
				return Weapon2D(missileLauncher, symbolImage, animation, size, grip);;
			} else {
				return Weapon2D(std::make_shared<Gun>(damage, timeBetweenShots, range, clipSize, shoot, reload), symbolImage, animation, size, grip);
			}
		}
		
		Unit2D loadUnit(GameInterface* gameInterface, std::string nameEntry, const GameData& gameData, bool infected) {
			GameDataEntry entry = gameData.getEntry(nameEntry);
			float mass = entry.getFloat("mass");
			float radius = entry.getFloat("radius");
			float life = entry.getFloat("life");
			float walkingSpeed = entry.getFloat("walkingSpeed");
			float runningSpeed = entry.getFloat("runningSpeed");
			float stamina = entry.getFloat("stamina");
			Animation moveA = entry.getAnimation("moveAnimation");
			Position grip;
			grip.x = entry.getFloat("moveImageGripX");
			grip.y = entry.getFloat("moveImageGripY");
			std::string weaponName = entry.getString("weapon");
						
			std::shared_ptr<Weapon> weapon;
			gameData.getEntry("weapons").iterateChilds("weapon", [&](GameDataEntry entry) {
				std::string name = entry.getString("name");
				if (name == weaponName) {
					weapon = loadWeapon2D(gameInterface, entry).clone();
					return false;
				}
				return true;
			});
			
			return Unit2D(mass, radius, life, walkingSpeed, runningSpeed, infected, weapon, moveA, grip);
		}

		Car2D loadCar(GameDataEntry& entry) {
			Animation animation = entry.getAnimation("moveAnimation");
			float mass = entry.getFloat("mass");
			float width = entry.getFloat("width");
			float length = entry.getFloat("length");
			float life = entry.getFloat("life");
			return Car2D(mass, life, width, length, animation);
		}

		ExplosionProperties loadExplosion(GameDataEntry& entry) {
			ExplosionProperties explosionProperties;
			explosionProperties.delay_ = entry.getFloat("timeDelay");
			explosionProperties.speed_ = entry.getFloat("speed");
			explosionProperties.blastRadius_ = entry.getFloat("blastRadius");
			explosionProperties.particle_ = entry.getTexture("particleImage");
			explosionProperties.sound_ = entry.getSound("sound");
			return explosionProperties;
		}

		Water loadWater(GameDataEntry& entry) {
			return Water(entry.getTexture("seeFloorImage"));
		}

		/*
		std::shared_ptr<Fog> loadFog(GameDataEntry& entry) {
			mw::Texture fog = entry.getTexture("image");
			float radius = entry.getFloat("radius");
			mw::Color color = entry.getColor("color");
			return std::make_shared<Fog>(fog, radius, color);
		}
		*/

	}

	ZombieGame::ZombieGame(const GameData& gameData) : engine_(*this,
		gameData.getEntry("settings").getInt("timeStepMS"),
		gameData.getEntry("settings").getFloat("impulseThreshold")), gameData_(gameData),
		human_(loadUnit(this, "human", gameData, false)),
		zombie_(loadUnit(this, "zombie", gameData_, true)),
		car_(zombie::loadCar(gameData_.getEntry("car"))),
		water_(loadWater(gameData.getEntry("water")))

		{

		init();
	}

	ZombieGame::~ZombieGame() {
	}

	void ZombieGame::init() {
		keyboard_ = DevicePtr(new InputKeyboard(SDLK_UP, SDLK_DOWN, SDLK_LEFT,
			SDLK_RIGHT, SDLK_SPACE, SDLK_r, SDLK_LSHIFT, SDLK_e));
		units_.reserve(200);
		cars_.reserve(40);
		clipsize_ = 0;
		bulletsInWeapon_ = 0;
		health_ = 0;
		scale_ = 1.f;
		lastSpawnTime_ = engine_.getTime();
		spawnPeriod_ = 0.5f;

		addKeyListener([&](gui::Component& component, const SDL_Event& keyEvent) {
			keyboard_->eventUpdate(keyEvent);
		});

		if (gameData_.getEntry("music").getBool("switch")) {
			music_ = gameData_.getEntry("music").getMusic("track");
			music_.setVolume(gameData_.getEntry("music").getFloat("volume"));
			music_.play(-1);
		}

		tree_ = gameData_.getEntry("tree").getSprite("image");
		wall_ = gameData_.getEntry("buildings").getSprite("wallImage");
		nbrUnits_ = 0;

		unitMaxLimit_ = gameData_.getEntry("settings").getInt("unitLimit");

		innerSpawnRadius_ = gameData_.getEntry("settings").getFloat("innerSpawnRadius");
		outerSpawnRadius_ = gameData_.getEntry("settings").getFloat("outerSpawnRadius");
		loadTerrain();

		// Load Weapons.
		gameData_.getEntry("weapons").iterateChilds("weapon", [&](GameDataEntry entry) {
			std::string weaponName = entry.getString("name");
			weapons_[weaponName] = zombie::loadWeapon2D(this, entry);
			return true;
		});
		
		//fog_ = zombie::loadFog(gameData.getEntry("fog"));
		//graphicSky_.push_back(fog_);
		explosionProperties_ = zombie::loadExplosion(gameData_.getEntry("explosion"));

		humanInjured_ = gameData_.getEntry("human").getAnimation("injuredAnimation");
		humanDie_ = gameData_.getEntry("human").getAnimation("dieAnimation");
		human_.setDieSound(gameData_.getEntry("human").getSound("dieSound"));
		human_.setHitSound(gameData_.getEntry("human").getSound("hitSound"));

		zombieInjured_ = gameData_.getEntry("zombie").getAnimation("injuredAnimation");
		zombieDie_ = gameData_.getEntry("zombie").getAnimation("dieAnimation");
		zombie_.setDieSound(gameData_.getEntry("zombie").getSound("dieSound"));
		zombie_.setHitSound(gameData_.getEntry("zombie").getSound("hitSound"));

		// Add human to engine.
		{
			State state(generatePosition(spawningPoints_), ORIGO, 0);
			engine_.setHuman(keyboard_, state, createUnit(human_));
			viewPosition_ = state.position_;
			refViewPosition_ = viewPosition_;
			++nbrUnits_;
		}

		// Add zombies to engine.
		int unitLevel = gameData_.getEntry("settings").getInt("unitLevel");
		for (int i = 0; i < 0; ++i) {
			State state(generatePosition(spawningPoints_), ORIGO, 0);
			engine_.add(state, createUnit(zombie_));
			++nbrUnits_;
		}
	
		// Add cars to engine.
		for (int i = 0; i < 1; ++i) {
			State state(generatePosition(spawningPoints_), ORIGO, 0);
			engine_.add(state, createCar(car_));
		}

		setBackgroundColor(0, 0.1f, 0);
		zombiesKilled_ = 0;
	}

	Unit2D* ZombieGame::createUnit(Unit2D& unit) {
		units_.emplace_back(unit);
		return &units_[units_.size() - 1];
	}
	
	Car2D* ZombieGame::createCar(Car2D& car) {
		cars_.emplace_back(car);
		return &cars_[units_.size() - 1];
	}
	
	void ZombieGame::updateEachCycle(Unit& unit, Unit& human) {
		Position diff = unit.getPosition() - human.getPosition();
		if (diff.LengthSquared() > outerSpawnRadius_*outerSpawnRadius_) {
			// Move to new postion and direction.
			Position spawnPoint = generatePosition(human.getPosition(), innerSpawnRadius_, outerSpawnRadius_);
			float angle = calculateAnglePointToPoint(spawnPoint, human.getPosition());
			unit.getBody()->SetTransform(spawnPoint, angle);			
		}
	}

	// Starts the game.
	void ZombieGame::startGame() {
		engine_.start();
	}

	void ZombieGame::draw(Uint32 deltaTime) {
		gui::Component::draw(deltaTime);
		
		viewPosition_ += 10 * deltaTime/1000.f * (refViewPosition_ - viewPosition_);
		/*
		if (fog_) {
			fog_->update(viewPosition_);
		}
		*/

		// Draw map centered around first human player.
		auto wPtr = getWindowMatrixPtr();
		wPtr->useShader();
		mw::Matrix44 old = wPtr->getModel();

		gui::Dimension dim = getSize();
		wPtr->setModel(old * mw::getTranslateMatrix(dim.width_*0.5f, dim.height_*0.5f) * mw::getScaleMatrix(50 * scale_, 50 * scale_) * mw::getTranslateMatrix(-viewPosition_.x, -viewPosition_.y));

		// Game is started?
		if (engine_.isStarted()) {
			terrain_.draw(deltaTime / 1000.f, wPtr);
			water_.drawSeeFloor(deltaTime / 1000.f, wPtr);
			drawGraphicList(graphicGround_, deltaTime / 1000.f, wPtr);
			engine_.update(deltaTime / 1000.f, getWindowMatrixPtr());
			drawGraphicList(graphicMiddle_, deltaTime / 1000.f, wPtr);
			water_.drawWaves(wPtr->getProjection() * wPtr->getModel());
			drawGraphicList(graphicSky_, deltaTime / 1000.f, wPtr);			

			removeDeadGraphicObjects(graphicGround_);
			removeDeadGraphicObjects(graphicMiddle_);
			removeDeadGraphicObjects(graphicSky_);
		} else {
			water_.drawSeeFloor(0, wPtr);
			terrain_.draw(0, wPtr);
			drawGraphicList(graphicGround_, 0, wPtr);
			engine_.update(0, getWindowMatrixPtr());
			drawGraphicList(graphicMiddle_, 0, wPtr);
			water_.drawWaves(wPtr->getProjection() * wPtr->getModel());
			drawGraphicList(graphicSky_, 0, wPtr);
		}

		wPtr->useShader();
		wPtr->setModel(old);
		
		State state = engine_.getHumanState();
		refViewPosition_ = state.position_ + 0.5 * state.velocity_;
	}

	void ZombieGame::updateEachCycle(Unit& human) {
		if (engine_.getTime() - lastSpawnTime_ > spawnPeriod_) {
			lastSpawnTime_ = engine_.getTime();

			if (unitMaxLimit_ > engine_.getNbrUnits()) {
				// Reduce spawnPeriod gradually
				//float angle = 2 * PI * random();
				Position spawnPoint = generatePosition(human.getPosition(), innerSpawnRadius_, outerSpawnRadius_);
				float angle = calculateAnglePointToPoint(spawnPoint, human.getPosition());
				State state(spawnPoint, ORIGO, angle);
				if (units_.size() < unitMaxLimit_) {
					engine_.add(state, createUnit(zombie_));
				} else {
					for (auto& unit : units_) {
						if (unit.getBody() != nullptr) {
							unit = zombie_;
							engine_.add(state, &unit);
						}
					}
				}
				++nbrUnits_;
			}
		}
		health_ = human.healthPoints();
		clipsize_ = human.getWeapon()->getClipSize();
		bulletsInWeapon_ = human.getWeapon()->getBulletsInWeapon();
	}

	void ZombieGame::zoom(float scale) {
		scale_ *= scale;
	}

	void ZombieGame::unitDied(Unit& unit) {
		--nbrUnits_;
		if (unit.isInfected()) {
			++zombiesKilled_;
			graphicGround_.push_back(std::make_shared<GraphicAnimation>(unit.getPosition(), unit.getDirection(), zombieDie_));
		} else {
			graphicGround_.push_back(std::make_shared<GraphicAnimation>(unit.getPosition(), unit.getDirection(), humanDie_));
		}
	}

	void ZombieGame::humanDied(Unit& unit) {
		graphicGround_.push_back(std::make_shared<GraphicAnimation>(unit.getPosition(), unit.getDirection(), humanDie_));
	}

	void ZombieGame::collision(float impulse, Car& car) {
	}

	void ZombieGame::collision(float impulse, Unit& unit) {
		unit.updateHealthPoint(-60 * impulse * engine_.getTimeStepMS() / 0.016f);
		if (unit.isDead()) {
			++zombiesKilled_;
			if (unit.isInfected()) {
				graphicGround_.push_back(std::make_shared<GraphicAnimation>(unit.getPosition(), unit.getDirection(), zombieDie_));
			} else {
				graphicGround_.push_back(std::make_shared<GraphicAnimation>(unit.getPosition(), unit.getDirection(), humanDie_));
			}
		} else {
			if (unit.isInfected()) {
				graphicGround_.push_back(std::make_shared<GraphicAnimation>(unit.getPosition(), unit.getDirection(), zombieInjured_));
			} else {
				graphicGround_.push_back(std::make_shared<GraphicAnimation>(unit.getPosition(), unit.getDirection(), humanInjured_));
			}
		}
	}

	void ZombieGame::collision(float impulse, Building& building) {
	}

	void ZombieGame::shotMissed(Position startPosition, Position hitPosition) {
		graphicMiddle_.push_back(std::make_shared<Shot>(startPosition, hitPosition));
	}

	void ZombieGame::shotHit(Position startPosition, Position hitPosition, Unit& unit) {
		graphicMiddle_.push_back(std::make_shared<Shot>(startPosition, hitPosition));
		graphicMiddle_.push_back(std::make_shared<GraphicAnimation>(unit.getPosition(), unit.getDirection(), zombieInjured_));
		if (unit.isInfected()) {
			graphicMiddle_.push_back(std::make_shared<GraphicAnimation>(unit.getPosition(), unit.getDirection(), zombieInjured_));
			// Temporary, just for fun!
			graphicSky_.push_back(std::make_shared<Explosion>(hitPosition, explosionProperties_));
		} else {
			graphicMiddle_.push_back(std::make_shared<GraphicAnimation>(unit.getPosition(), unit.getDirection(), humanInjured_));
		}
	}

	void ZombieGame::explosion(Position position, float explosionRadius) {
		auto tmp = explosionProperties_;
		tmp.blastRadius_ = explosionRadius;
		graphicSky_.push_back(std::make_shared<Explosion>(position, tmp));
	}

	void ZombieGame::removedFromWorld(Unit& unit) {
	}

	void ZombieGame::loadTerrain() {
		auto mapEntry = gameData_.getMapEntry();
		std::string name = mapEntry.getString("name");
		mapEntry.getChildEntry("objects").iterateChilds("object", [&](GameDataEntry entry) {
			std::string geom(entry.getString("geom"));
			if (entry.isAttributeEqual("type", "building")) {
				engine_.add(new Building2D(loadPolygon(geom), wall_, wall_, wall_));
			} else if (entry.isAttributeEqual("type", "water")) {
				auto triangle = loadPolygon(geom);
				water_.addTriangle(triangle[0], triangle[1], triangle[2]);
			} else if (entry.isAttributeEqual("type", "road")) {
				terrain_.addRoad(loadPolygon(geom));
			} else if (entry.isAttributeEqual("type", "tree")) {
				engine_.add(new Tree2D(loadPoint(geom), tree_));
			} else if (entry.isAttributeEqual("type", "spawningpoint")) {			
				spawningPoints_.push_back(loadPoint(geom));
			} else if (entry.isAttributeEqual("type", "carSpawningpoint")) {
				//carSpawningPoints_.push_back(loadPoint(geom));
			}
			return true;
		});
	}

} // Namespace zombie.
