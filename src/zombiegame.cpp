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

		// Return a random postion between the defined outer and inner circle centered in position.
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

	}

	ZombieGame::ZombieGame(GameDataEntry zombieEntry) : engine_(*this,
		zombieEntry.getEntry("settings timeStepMS").getInt(),
		zombieEntry.getEntry("settings impulseThreshold").getFloat()), zombieEntry_(zombieEntry),
		human_(loadUnit(this, "human", zombieEntry, false)),
		zombie_(loadUnit(this, "zombie", zombieEntry, true)),
		car_(zombie::loadCar(zombieEntry.getEntry("car"))),
		water_(loadWater(zombieEntry.getEntry("water"))),
		frame_(0),
		fps_(60),
		lastFramTime_(0)

		{

		init();
	}

	ZombieGame::~ZombieGame() {
		zombieEntry_.save();
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

		if (zombieEntry_.getEntry("music switch").getBool()) {
			music_ = zombieEntry_.getEntry("music track").getMusic();
			music_.setVolume(zombieEntry_.getEntry("music volume").getFloat());
			music_.play(-1);
		}

		tree_ = zombieEntry_.getEntry("tree image").getSprite();
		wall_ = zombieEntry_.getEntry("buildings wallImage").getSprite();
		nbrUnits_ = 0;

		unitMaxLimit_ = zombieEntry_.getEntry("settings unitLimit").getInt();

		innerSpawnRadius_ = zombieEntry_.getEntry("settings innerSpawnRadius").getFloat();
		outerSpawnRadius_ = zombieEntry_.getEntry("settings outerSpawnRadius").getFloat();
		loadTerrain();
		
		//fog_ = zombie::loadFog(gameData.getEntry("fog"));
		//graphicSky_.push_back(fog_);
		explosionProperties_ = zombie::loadExplosion(zombieEntry_.getEntry("explosion"));

		humanInjured_ = zombieEntry_.getEntry("human injuredAnimation").getAnimation();
		humanDie_ = zombieEntry_.getEntry("human dieAnimation").getAnimation();
		human_.setDieSound(zombieEntry_.getEntry("human dieSound").getSound());
		human_.setHitSound(zombieEntry_.getEntry("human hitSound").getSound());

		zombieInjured_ = zombieEntry_.getEntry("zombie injuredAnimation").getAnimation();
		zombieDie_ = zombieEntry_.getEntry("zombie dieAnimation").getAnimation();
		zombie_.setDieSound(zombieEntry_.getEntry("zombie dieSound").getSound());
		zombie_.setHitSound(zombieEntry_.getEntry("zombie hitSound").getSound());

		// Add human to engine.
		{
			State state(generatePosition(spawningPoints_), ORIGO, 0);
			engine_.setHuman(keyboard_, state, createUnit(human_));
			viewPosition_ = state.position_;
			refViewPosition_ = viewPosition_;
			++nbrUnits_;
		}

		// Add zombies to engine.
		int unitLevel = zombieEntry_.getEntry("settings unitLevel").getInt();
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
		++frame_;
		lastFramTime_ += deltaTime;
		if (frame_ == 60) {
			fps_ = frame_ / lastFramTime_ * 1000;
			frame_ = 0;
			lastFramTime_ = 0;
		}

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
				if ((int) units_.size() < unitMaxLimit_) {
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
		auto entry = GameDataEntry(zombieEntry_.getEntry("settings map").getString());
		entry = entry.getEntry("map objects object");
		while (entry.hasData()) {
			std::string geom(entry.getChildEntry("geom").getString());
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
			entry = entry.getSibling("object");
		}
	}

	float ZombieGame::getFps() const {
		return fps_;
	}

} // Namespace zombie.
