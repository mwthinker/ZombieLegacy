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

		void drawGraphicList(std::list<std::shared_ptr<Graphic>>& list, float deltaTime) {
			for (auto ob : list) {
				ob->draw(deltaTime);
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
		
		std::unique_ptr<Unit2D> loadUnit(GameInterface* gameInterface, GameDataEntry& entry, bool infected, std::map<std::string, Weapon2D>& weapons) {
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
			return std::unique_ptr<Unit2D>(new Unit2D(mass, radius, life, walkingSpeed, runningSpeed, infected, weapons[weaponName].clone(), moveA, grip));
		}

		std::unique_ptr<Car2D> loadCar(GameDataEntry& entry) {
			Animation animation = entry.getAnimation("moveAnimation");
			float mass = entry.getFloat("mass");
			float width = entry.getFloat("width");
			float length = entry.getFloat("length");
			float life = entry.getFloat("life");
			return std::unique_ptr<Car2D>(new Car2D(mass, life, width, length, animation));
		}

		std::shared_ptr<Explosion> loadExplosion(GameDataEntry& entry) {
			mw::Texture particle = entry.getTexture("particleImage");
			mw::Sprite shockwave = entry.getSprite("shockwaveImage");
			mw::Sprite emitter = entry.getSprite("emitterImage");
			mw::Sound sound = entry.getSound("sound");
			return std::make_shared<Explosion>(particle, shockwave, emitter, sound);
		}

		std::shared_ptr<Fog> loadFog(GameDataEntry& entry) {
			mw::Texture fog = entry.getTexture("image");
			float radius = entry.getFloat("radius");
			mw::Color color = entry.getColor("color");
			return std::make_shared<Fog>(fog, radius, color);
		}		

	}

	ZombieGame::ZombieGame(const GameData& gameData) : engine_(*this, 
		gameData.getEntry("settings").getInt("timeStepMS"),
		gameData.getEntry("settings").getFloat("impulseThreshold")), gameData_(gameData) {
		keyboard_ = DevicePtr(new InputKeyboard(SDLK_UP, SDLK_DOWN, SDLK_LEFT,
			SDLK_RIGHT, SDLK_SPACE, SDLK_r, SDLK_LSHIFT, SDLK_e));
		clipsize_ = 0;
		bulletsInWeapon_ = 0;
		health_ = 0;
		scale_ = 1.f;
		lastSpawnTime_ = engine_.getTime();
		spawnPeriod_ = 0.5f;

		addKeyListener([&](gui::Component& component, const SDL_Event& keyEvent) {
			keyboard_->eventUpdate(keyEvent);
		});

		if (gameData.getEntry("music").getBool("switch")) {
			music_ = gameData.getEntry("music").getMusic("track");
			music_.setVolume(gameData.getEntry("music").getFloat("volume"));
			music_.play(-1);
		}
		
		tree_ = gameData.getEntry("tree").getSprite("image");
		wall_ = gameData.getEntry("buildings").getSprite("wallImage");
		nbrUnits_ = 0;

		unitMaxLimit_ = gameData.getEntry("settings").getInt("unitLimit");

		innerSpawnRadius_ = gameData.getEntry("settings").getFloat("innerSpawnRadius");
		outerSpawnRadius_ = gameData.getEntry("settings").getFloat("outerSpawnRadius");
		loadTerrain();

		// Load Weapons.
		gameData.getEntry("weapons").iterateChilds("weapon", [&](GameDataEntry entry) {
			std::string weaponName = entry.getString("name");
			weapons_[weaponName] = zombie::loadWeapon2D(this, entry);
			return true;
		});
		
		human_ = zombie::loadUnit(this, gameData.getEntry("human"), false, weapons_);
		zombie_ = zombie::loadUnit(this, gameData.getEntry("zombie"), true, weapons_);
		car_ = zombie::loadCar(gameData.getEntry("car"));
		fog_ = zombie::loadFog(gameData.getEntry("fog"));
		graphicHeaven_.push_back(fog_);
		explosion_ = zombie::loadExplosion(gameData.getEntry("explosion"));

		humanInjured_ = gameData.getEntry("human").getAnimation("injuredAnimation");
		humanDie_ = gameData.getEntry("human").getAnimation("dieAnimation");
		human_->setDieSound(gameData.getEntry("human").getSound("dieSound"));
		human_->setHitSound(gameData.getEntry("human").getSound("hitSound"));

		zombieInjured_ = gameData.getEntry("zombie").getAnimation("injuredAnimation");
		zombieDie_ = gameData.getEntry("zombie").getAnimation("dieAnimation");
		zombie_->setDieSound(gameData.getEntry("zombie").getSound("dieSound"));
		zombie_->setHitSound(gameData.getEntry("zombie").getSound("hitSound"));

		// Add human to engine.
		{
			State state(generatePosition(spawningPoints_), ORIGO, 0);
			Unit* human = new Unit2D(*human_);
			engine_.setHuman(keyboard_, state, human);
			viewPosition_ = state.position_;
			refViewPosition_ = viewPosition_;
			++nbrUnits_;
		}

		// Add zombies to engine.
		int unitLevel = gameData.getEntry("settings").getInt("unitLevel");
		for (int i = 0; i < 0; ++i) {
			Unit* zombie = new Unit2D(*zombie_);
			State state(generatePosition(spawningPoints_), ORIGO, 0);
			engine_.add(state, zombie);
			++nbrUnits_;
		}

		// Add cars to engine.
		for (int i = 0; i < 15; ++i) {
			State state(generatePosition(spawningPoints_), ORIGO, 0);
			engine_.add(state, new Car2D(*car_));
		}

		setBackgroundColor(0, 0.1f, 0);
		zombiesKilled_ = 0;
	}

	ZombieGame::~ZombieGame() {
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
		if (fog_) {
			fog_->update(viewPosition_);
		}

		// Draw map centered around first human player.
		glPushMatrix();
		gui::Dimension dim = getSize();
		glTranslated(dim.width_*0.5f, dim.height_*0.5f, 0);
		glScale2f(50);
		glScale2f(scale_);
		glTranslate2f(-viewPosition_);

		// Game is started?
		if (engine_.isStarted()) {
			terrain_.draw(deltaTime / 1000.f);
			drawGraphicList(graphicGround_, deltaTime / 1000.f);
			engine_.update(deltaTime / 1000.f);
			drawGraphicList(graphicMiddle_, deltaTime / 1000.f);
			drawGraphicList(graphicHeaven_, deltaTime / 1000.f);

			removeDeadGraphicObjects(graphicGround_);
			removeDeadGraphicObjects(graphicMiddle_);
			removeDeadGraphicObjects(graphicHeaven_);
		} else {
			terrain_.draw(0);
			drawGraphicList(graphicGround_, 0);
			engine_.update(0);
			drawGraphicList(graphicMiddle_, 0);
			drawGraphicList(graphicHeaven_, 0);
		}

		glPopMatrix();
		
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
				Unit2D* unit = new Unit2D(*zombie_);
				engine_.add(state, unit);
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
			graphicHeaven_.push_back(std::make_shared<Explosion>(hitPosition, unit.getDirection(), *explosion_));
		} else {
			graphicMiddle_.push_back(std::make_shared<GraphicAnimation>(unit.getPosition(), unit.getDirection(), humanInjured_));
		}
	}

	void ZombieGame::explosion(Position position, float explosionRadius) {
		graphicHeaven_.push_back(std::make_shared<Explosion>(position, 0.f, *explosion_));
	}

	void ZombieGame::loadTerrain() {
		auto mapEntry = gameData_.getMapEntry();
		std::string name = mapEntry.getString("name");
		mapEntry.getChildEntry("objects").iterateChilds("object", [&](GameDataEntry entry) {
			std::string geom(entry.getString("geom"));
			if (entry.isAttributeEqual("type", "building")) {
				engine_.add(new Building2D(loadPolygon(geom), wall_, wall_, wall_));
			} else if (entry.isAttributeEqual("type", "water")) {
				terrain_.addWater(loadPolygon(geom));
			} else if (entry.isAttributeEqual("type", "road")) {
				terrain_.addRoad(loadPolygon(geom));
			} else if (entry.isAttributeEqual("type", "tree")) {
				engine_.add(new Tree2D(loadPoint(geom), tree_));
			} else if (entry.isAttributeEqual("type", "spawningpoint")) {			
				spawningPoints_.push_back(loadPoint(geom));
			}
			return true;
		});
	}

} // Namespace zombie.
