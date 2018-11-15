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
#include "tree2d.h"
#include "gun.h"
#include "missilelauncher2d.h"
#include "humanplayer.h"
#include "zombiebehavior.h"
#include "gamedata.h"

#include <mw/opengl.h>
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
		
		Position generatePosition(const std::vector<Position>& positions) {
			if (positions.size() > 0) {
				return positions[randomInt(0, positions.size() - 1)];
			}
			return ZERO;
		}
		
		template <class Vector>
		void activateFirstFreeSlot(Vector& v, Position p, float angle, const Animation& dieAnimation) {
			// Activate the first free animation slot.
			for (GraphicAnimation& animation : v) {
				if (animation.toBeRemoved()) {
					animation.restart(p, angle, dieAnimation);
					break;
				}
			}
		}

	}

	ZombieGame::ZombieGame() : engine_(*this,
		GameData::getInstance().getSettingsImpulseThreshold()),
		water_(GameData::getInstance().getWaterSeeFloorImage()),
		frame_(0),
		meanFrameTime_(16),
		lastFramTime_(0),
		timeStep_(GameData::getInstance().getSettingsTimeStep()),
		accumulator_(0),
		gameShader_("game.ver.glsl", "game.fra.glsl"),
		waterShader_("water.ver.glsl", "water.fra.glsl"),
		buildingShader_("building.ver.glsl", "building.fra.glsl"),
		started_(false)

		{

		zombieGameInit();
	}

	ZombieGame::~ZombieGame() {
		//zombieEntry_.save();
	}

	void ZombieGame::zombieGameInit() {
		keyboard_ = DevicePtr(new InputKeyboard(SDLK_UP, SDLK_DOWN, SDLK_LEFT,
			SDLK_RIGHT, SDLK_SPACE, SDLK_r, SDLK_LSHIFT, SDLK_e));		
		clipsize_ = 0;
		bulletsInWeapon_ = 0;
		health_ = 0;
		scale_ = 1.f;
		lastSpawnTime_ = (float) engine_.getTime();
		spawnPeriod_ = 0.5f;

		addKeyListener([&](gui::Component& component, const SDL_Event& keyEvent) {
			keyboard_->eventUpdate(keyEvent);
		});

		if (GameData::getInstance().isMusicOn()) {
			music_ = GameData::getInstance().getMusicTrack();
			music_.setVolume(GameData::getInstance().getMusicVolume());
			music_.play(-1);
		}

		tree_ = GameData::getInstance().getTreeImage();
		wall_ = GameData::getInstance().getBuildingWallImage();
		nbrUnits_ = 0;

		unitMaxLimit_ = GameData::getInstance().getSettingsUnitLimit();

		innerSpawnRadius_ = GameData::getInstance().getSettingsInnerSpawnRadius();
		outerSpawnRadius_ = GameData::getInstance().getSettingsOuterSpawnRadius();
		
		loadTerrain();
		
		explosionProperties_ = GameData::getInstance().getExplosionProperties();
		
		humanProperties_ = GameData::getInstance().getHumanProperties();
		zombieProperties_ = GameData::getInstance().getZombieProperties();

		Unit2D human(loadUnit(this, humanProperties_, false));
		Unit2D zombie(loadUnit(this, zombieProperties_, true));

		// Add human to engine.
		{
			State state(Position(85,120), ORIGO, 0);
			//Position p = generatePosition(spawningPoints_);
			//State state(Position(200,200), ORIGO, 0);
			Unit* unit = units_.pushBack(human);
			engine_.add(unit);
			unit->setState(state);
			unit->setActive(true);
			unit->setAwake(true);
			players_.push_back(std::unique_ptr<HumanPlayer>(new HumanPlayer(keyboard_, unit)));
			viewPosition_ = state.position_;
			refViewPosition_ = viewPosition_;
			++nbrUnits_;
		}

		// Add zombies to engine.
		calculateValidSpawningPoints(units_[0]);
		unsigned int unitLevel = GameData::getInstance().getSettingsUnitLevel();
		for (unsigned int i = 1; i <= unitLevel && i < units_.getMaxSize(); ++i) {
			Position p = generatePosition(vaildSpawningPoints_);
			float angle = calculateAnglePointToPoint(p, units_[0].getPosition());
			State state(p, ORIGO, angle);
			Unit* unit = units_.pushBack(zombie);
			engine_.add(unit);
			unit->setState(state);
			unit->setActive(true);
			unit->setAwake(true);
			players_.push_back(std::unique_ptr<ZombieBehavior>(new ZombieBehavior(unit)));
		}
	
		// Add cars to engine.
		Car2D car; // (zombie::loadCar(zombieEntry_.getDeepChildEntry("car")));
		for (unsigned int i = 0; i < 8 && i < units_.getMaxSize(); ++i) {
			State state(Position(85,130), ORIGO, 0);
			Car* c = cars_.pushBack(car);
			engine_.add(c);
			c->setState(state);
			c->setActive(true);
			c->setAwake(true);
		}

		// Add missile to engine.
		Missile2D missile = loadMissile2D(this, GameData::getInstance().getMissileProperties());
		for (unsigned int i = 0; i < 10 && i < units_.getMaxSize(); ++i) {
			engine_.add(missiles_.emplaceBack(missile));
		}

		setBackgroundColor(0, 0.1f, 0);
		zombiesKilled_ = 0;

		drawBuildings_.createVBO(buildings_, wall_.getTexture());
	}

	void ZombieGame::calculateValidSpawningPoints(Unit& human) {
		vaildSpawningPoints_.clear();
		float inner = 10;
		float outer = 200;
		Position humanPos = human.getPosition();
		for (Position p : spawningPoints_) {
			Position diff = p - humanPos;
			if (diff.LengthSquared() > inner*inner && diff.LengthSquared() < outer*outer) {
				// Spawningpoint is valid!
				vaildSpawningPoints_.push_back(p);
			}
		}
	}
	
	void ZombieGame::moveUnits(Unit& unit, Unit& human) {
		Position diff = unit.getPosition() - human.getPosition();
		double inner = 10;
		double outer = 200;
		if (diff.LengthSquared() > outer * outer) {
			// Move unit if possible.
			if (vaildSpawningPoints_.size() > 0) {
				Position p = vaildSpawningPoints_[randomInt(0, vaildSpawningPoints_.size() - 1)];
				float angle = calculateAnglePointToPoint(p, human.getPosition());
				State state(p, ORIGO, angle);
				unit.setState(state);
			}
			else {
				// Deactivate.
				unit.setActive(false);
				unit.setAwake(false);
			}
		}
	}

	// Starts the game.
	void ZombieGame::startGame() {
		started_ = true;
	}

	void ZombieGame::draw(const gui::Graphic& graphic, double deltaTime) {
		gui::Component::draw(graphic, deltaTime);

		++frame_;
		lastFramTime_ += deltaTime;
		if (frame_ == 60) {
			meanFrameTime_ = lastFramTime_ / 60.0;
			frame_ = 0;
			lastFramTime_ = 0;
		}
		
		viewPosition_ += 10 * (float) deltaTime * (refViewPosition_ - viewPosition_);
		
		if (started_) {
			updateGame(deltaTime);
		}

		drawGame(graphic.getProjectionMatrix(), deltaTime);
				
		refViewPosition_ = humanState_.position_ + 0.5 * humanState_.velocity_;
	}

	void ZombieGame::updateGame(double deltaTime) {
		if (deltaTime > 0.25) {
			// To avoid spiral of death.
			deltaTime = 0.25;
		}

		// Previous state for the human in the physic loop.
		State previousState;
		bool physicRan = false;

		accumulator_ += deltaTime;
		while (accumulator_ >= timeStep_) {
			accumulator_ -= timeStep_;
			previousState = units_[0].getState();
			physicRan = true;
			makeGameStep();
		}

		if (physicRan) {
			const float alpha = (float) (accumulator_ / timeStep_);
			humanState_ = humanState_ = units_[0].getState();
			humanState_.position_ = alpha * humanState_.position_ + (1.f - alpha) * previousState.position_;
			humanState_.velocity_ = alpha * humanState_.velocity_ + (1.f - alpha) * previousState.velocity_;
		}
	}

	void ZombieGame::makeGameStep() {
		float time = (float) engine_.getTime();

		// Update all game entities.
		for (Car2D& car : cars_) {
			if (car.getBody() != nullptr) {
				car.updatePhysics(time, timeStep_);
			}
		}
		
		// filter out the valid spawningpoints
		calculateValidSpawningPoints(units_[0]);
		
		for (Unit& unit : units_) {
			if (unit.isActive()) {
				// move the unit if to far away
				moveUnits(unit, units_[0]);
				unit.updatePhysics(time, timeStep_);
			}
		}

		for (Missile& missile : missiles_) {
			if (missile.isActive()) {
				missile.updatePhysics(time, timeStep_);
			}
		}

		// Update the human and ai input.
		for (auto& player : players_) {
			player->updateInput(time, timeStep_);
		}

		// Update the engine.
		engine_.update(timeStep_);
	}

	void ZombieGame::drawGame(const Matrix44& projection, double deltaTime) {
		// Draw map centered around first human player.
		gui::Dimension dim = getSize();
		Matrix44 matrix = getModelMatrix();
		mw::translate2D(matrix, dim.width_*0.5f, dim.height_*0.5f);
		mw::scale2D(matrix, 50 * scale_, 50 * scale_);
		
		// Update global uniform data to the shaders used.
		waterShader_.glUseProgram();
		waterShader_.setGlProjectionMatrixU(projection);
		waterShader_.setGlModelMatrixU(matrix);
		waterShader_.setGlGlobalCenterPositionU(viewPosition_);

		buildingShader_.glUseProgram();
		buildingShader_.setGlProjectionMatrixU(projection);
		buildingShader_.setGlModelMatrixU(matrix);
		buildingShader_.setGlGlobalCenterPositionU(viewPosition_);
		buildingShader_.setGlGlobalHumanPositionU(humanState_.position_);
		
		gameShader_.useProgram();
		gameShader_.setGlProjectionMatrixU(projection);
		gameShader_.setGlModelMatrixU(matrix);
		gameShader_.setGlGlobalCenterPositionU(viewPosition_);
		
		// Game has not started?
		if (!started_) {
			deltaTime = 0.0;
		}

		mw::checkGlError();
		water_.drawSeeFloor((float) deltaTime, gameShader_);
		water_.drawWaves((float) deltaTime, waterShader_);

		terrain_.draw((float) deltaTime, gameShader_);

		drawBuildings_.drawWalls((float) accumulator_, (float) deltaTime, buildingShader_);
		
		gameShader_.useProgram();
		for (GraphicAnimation& animation : graphicAnimations_) {
			if (!animation.toBeRemoved()) {
				animation.draw((float) deltaTime, gameShader_);
			}
		}

		for (Missile2D& missile : missiles_) {
			if (missile.isActive()) {
				missile.draw((float) accumulator_, (float) deltaTime, gameShader_);
			}
		}
		mw::checkGlError();
		for (Car2D& car : cars_) {
			if (car.isActive()) {
				car.draw(accumulator_, deltaTime, gameShader_);
			}
		}
		mw::checkGlError();

		for (Unit2D& unit : units_) {
			if (unit.isActive()) {
				unit.draw((float) accumulator_, (float) deltaTime, gameShader_);
			}
		}

		drawBuildings_.drawRoofs((float) accumulator_, (float) deltaTime, buildingShader_);
		
		for (auto& explosion : explosions_) {
			if (!explosion.toBeRemoved()) {
				explosion.draw((float) deltaTime, gameShader_);
			}
		}
	}

	void ZombieGame::zoom(float scale) {
		scale_ *= scale;
	}

	void ZombieGame::unitDied(Unit& unit) {
		--nbrUnits_;

		Animation* dieAnimation = &humanProperties_.dieAnimation_;
		if (unit.isInfected()) {
			++zombiesKilled_;
			dieAnimation = &zombieProperties_.dieAnimation_;
		}

		activateFirstFreeSlot(graphicAnimations_, unit.getPosition(), unit.getDirection(), *dieAnimation);

		unit.setActive(false);
		unit.setAwake(false);
	}

	void ZombieGame::collision(float impulse, Car& car) {
	}

	void ZombieGame::collision(float impulse, Unit& unit) {
		unit.updateHealthPoint(-60 * impulse * (float) (timeStep_ / 0.016f));
	}

	void ZombieGame::collision(float impulse, Building& building) {
	}

	void ZombieGame::shotMissed(Position startPosition, Position hitPosition) {
		//graphicMiddle_.push_back(std::make_shared<Shot>(startPosition, hitPosition));
	}

	void ZombieGame::shotHit(Position startPosition, Position hitPosition, Unit& unit) {
		Animation* animation = &humanProperties_.injuredAnimation_;
		if (unit.isInfected()) {
			++zombiesKilled_;
			animation = &zombieProperties_.injuredAnimation_;
		}

		activateFirstFreeSlot(graphicAnimations_, unit.getPosition(), unit.getDirection(), *animation);
	}

	void ZombieGame::explosion(Position position, float explosionRadius) {
		auto tmp = explosionProperties_;
		for (auto& explosion : explosions_) {
			if (explosion.toBeRemoved()) {
				explosion.restart(position, explosionRadius, tmp);
				// Only draw one explosion!
				break;
			}
		}
	}

	void ZombieGame::shot(Unit& shooter, float speed, float explodeTime, float damage, float explosionRadius, float force) {
		for (auto& missile : missiles_) {
			if (!missile.isActive()) {
				float x = shooter.getGrip().x;
				float y = shooter.getGrip().y;
				float angle = shooter.getDirection();
				float s = std::sin(angle);
				float c = std::cos(angle);
				Position release = Position(c * x - s * y, s * x + c * y);  // Rotates the vector.
				missile.create(shooter.getPosition() + release,
					angle, speed, explodeTime, damage, explosionRadius, force);
				// Only send one missile.
				break;
			}
		}
	}

	void ZombieGame::removedFromWorld(Unit& unit) {
	}

	void ZombieGame::loadTerrain() {
		MapProperties properties = GameData::getInstance().loadMapProperties();

		for (MapProperties::Object& ob : properties.positions_) {
			switch (ob.type_) {
				case MapProperties::BUILDING:
				{
					Building* b = buildings_.emplaceBack(ob.geom_[0], ob.geom_[1], ob.geom_[2], ob.geom_[3], wall_, wall_, wall_);
					engine_.add(b);
					b->setActive(true);
					b->setAwake(true);
				}
					break;
				case MapProperties::WATER:
					water_.addTriangle(ob.geom_[0], ob.geom_[1], ob.geom_[2]);
					break;
				case MapProperties::GRASS:
					terrain_.addGrass(ob.geom_[0], ob.geom_[1], ob.geom_[2]);
					break;
				case MapProperties::TILEPOINT:
					//terrain_.addRoad(entry);
					break;
				case MapProperties::TREE:
					//engine_.add(new Tree2D(loadPoint(ob.geom_), tree_));
					break;
				case MapProperties::SPAWNINGPOINT:
					spawningPoints_.push_back(ob.geom_[0]);
					break;
				case MapProperties::CAR_SPAWNINGPOINT:
					break;
			}
		}
	}

	double ZombieGame::getMeanFrameTime() const {
		return meanFrameTime_;
	}

} // Namespace zombie.
