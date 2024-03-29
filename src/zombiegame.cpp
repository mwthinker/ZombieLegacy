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
#include "zombieentry.h"
#include "humanplayer.h"
#include "zombiebehavior.h"

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

	ZombieGame::ZombieGame(ZombieEntry zombieEntry) : engine_(*this,
		zombieEntry.getDeepChildEntry("settings impulseThreshold").getFloat()), zombieEntry_(zombieEntry),
		water_(loadWater(zombieEntry.getDeepChildEntry("water"))),
		frame_(0),
		meanFrameTime_(0.0166f),
		lastFramTime_(0),
		timeStep_(zombieEntry.getDeepChildEntry("settings timeStepMS").getInt() / 1000.f),
		accumulator_(0),
		gameShader_("game.ver.glsl", "game.fra.glsl"),
		waterShader_("water.ver.glsl", "water.fra.glsl"),
		buildingShader_("building.ver.glsl", "building.fra.glsl"),
		started_(false)

		{

		init();
	}

	ZombieGame::~ZombieGame() {
		zombieEntry_.save();
	}

	void ZombieGame::init() {
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

		if (zombieEntry_.getDeepChildEntry("music switch").getBool()) {
			music_ = zombieEntry_.getDeepChildEntry("music track").getMusic();
			music_.setVolume(zombieEntry_.getDeepChildEntry("music volume").getFloat());
			music_.play(-1);
		}

		tree_ = zombieEntry_.getDeepChildEntry("tree image").getSprite();
		wall_ = zombieEntry_.getDeepChildEntry("buildings wallImage").getSprite();
		nbrUnits_ = 0;

		unitMaxLimit_ = zombieEntry_.getDeepChildEntry("settings unitLimit").getInt();

		innerSpawnRadius_ = zombieEntry_.getDeepChildEntry("settings innerSpawnRadius").getFloat();
		outerSpawnRadius_ = zombieEntry_.getDeepChildEntry("settings outerSpawnRadius").getFloat();
		
		terrain_.loadRoadSprites(zombieEntry_.getDeepChildEntry("roads"));
		loadTerrain();
		
		explosionProperties_ = zombie::loadExplosion(zombieEntry_.getDeepChildEntry("explosion"));

		humanInjured_ = zombieEntry_.getDeepChildEntry("human injuredAnimation").getAnimation();
		humanDie_ = zombieEntry_.getDeepChildEntry("human dieAnimation").getAnimation();
		Unit2D human(loadUnit(this, "human", zombieEntry_, false));
		human.setDieSound(zombieEntry_.getDeepChildEntry("human dieSound").getSound());
		human.setHitSound(zombieEntry_.getDeepChildEntry("human hitSound").getSound());
		
		zombieInjured_ = zombieEntry_.getDeepChildEntry("zombie injuredAnimation").getAnimation();
		zombieDie_ = zombieEntry_.getDeepChildEntry("zombie dieAnimation").getAnimation();
		Unit2D zombie(loadUnit(this, "zombie", zombieEntry_, true));
		zombie.setDieSound(zombieEntry_.getDeepChildEntry("zombie dieSound").getSound());
		zombie.setHitSound(zombieEntry_.getDeepChildEntry("zombie hitSound").getSound());

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
		unsigned int unitLevel = zombieEntry_.getDeepChildEntry("settings unitLevel").getInt();
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
		Car2D car(zombie::loadCar(zombieEntry_.getDeepChildEntry("car")));
		for (unsigned int i = 0; i < 8 && i < units_.getMaxSize(); ++i) {
			State state(Position(85,130), ORIGO, 0);
			Car* c = cars_.pushBack(car);
			engine_.add(c);
			c->setState(state);
			c->setActive(true);
			c->setAwake(true);
		}

		// Add missile to engine.
		Missile2D missile(loadMissile2D(this, zombieEntry_.getDeepChildEntry("equipment missile")));
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

	void ZombieGame::draw(Uint32 deltaTime) {
		gui::Component::draw(deltaTime);

		++frame_;
		lastFramTime_ += deltaTime;
		if (frame_ == 60) {
			meanFrameTime_ = lastFramTime_/1000.f;
			frame_ = 0;
			lastFramTime_ = 0;
		}
		
		viewPosition_ += 10 * deltaTime/1000.f * (refViewPosition_ - viewPosition_);
		
		if (started_) {
			updateGame(deltaTime / 1000.f);
		}

		drawGame(deltaTime / 1000.f);
				
		refViewPosition_ = humanState_.position_ + 0.5 * humanState_.velocity_;
	}

	void ZombieGame::updateGame(float deltaTime) {
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
			const float alpha = accumulator_ / timeStep_;
			humanState_ = humanState_ = units_[0].getState();
			humanState_.position_ = alpha * humanState_.position_ + (1.f - alpha) * previousState.position_;
			humanState_.velocity_ = alpha * humanState_.velocity_ + (1.f - alpha) * previousState.velocity_;
		}
	}

	void ZombieGame::makeGameStep() {
		float time = engine_.getTime();

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

	void ZombieGame::drawGame(float deltaTime) {
		// Draw map centered around first human player.
		gui::Dimension dim = getSize();
		mw::Matrix44 matrix = getModelMatrix();
		mw::translate2D(matrix, dim.width_*0.5f, dim.height_*0.5f);
		mw::scale2D(matrix, 50 * scale_, 50 * scale_);
		
		mw::Matrix44 projection = getProjectionMatrix();

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
		
		gameShader_.glUseProgram();
		gameShader_.setGlProjectionMatrixU(projection);
		gameShader_.setGlModelMatrixU(matrix);
		gameShader_.setGlGlobalCenterPositionU(viewPosition_);
		
		// Game has not started?
		if (!started_) {
			deltaTime = 0;
		}

		mw::checkGlError();
		water_.drawSeeFloor(deltaTime, gameShader_);
		water_.drawWaves(deltaTime, waterShader_);

		terrain_.draw(deltaTime, gameShader_);

		drawBuildings_.drawWalls(accumulator_, deltaTime, buildingShader_);
		
		gameShader_.glUseProgram();
		for (GraphicAnimation& animation : graphicAnimations_) {
			if (!animation.toBeRemoved()) {
				animation.draw(deltaTime, gameShader_);
			}
		}

		for (Missile2D& missile : missiles_) {
			if (missile.isActive()) {
				missile.draw(accumulator_, deltaTime, gameShader_);
			}
		}
		
		for (Car2D& car : cars_) {
			if (car.isActive()) {
				car.draw(accumulator_, deltaTime, gameShader_);
			}
		}

		for (Unit2D& unit : units_) {
			if (unit.isActive()) {
				unit.draw(accumulator_, deltaTime, gameShader_);
			}
		}

		drawBuildings_.drawRoofs(accumulator_, deltaTime, buildingShader_);
		
		for (auto& explosion : explosions_) {
			if (!explosion.toBeRemoved()) {
				explosion.draw(deltaTime, gameShader_);
			}
		}
	}

	void ZombieGame::zoom(float scale) {
		scale_ *= scale;
	}

	void ZombieGame::unitDied(Unit& unit) {
		--nbrUnits_;

		Animation* dieAnimation = &humanDie_;
		if (unit.isInfected()) {
			++zombiesKilled_;
			dieAnimation = &zombieDie_;
		}

		activateFirstFreeSlot(graphicAnimations_, unit.getPosition(), unit.getDirection(), *dieAnimation);

		unit.setActive(false);
		unit.setAwake(false);
	}

	void ZombieGame::collision(float impulse, Car& car) {
	}

	void ZombieGame::collision(float impulse, Unit& unit) {
		unit.updateHealthPoint(-60 * impulse * timeStep_ / 0.016f);
	}

	void ZombieGame::collision(float impulse, Building& building) {
	}

	void ZombieGame::shotMissed(Position startPosition, Position hitPosition) {
		//graphicMiddle_.push_back(std::make_shared<Shot>(startPosition, hitPosition));
	}

	void ZombieGame::shotHit(Position startPosition, Position hitPosition, Unit& unit) {
		Animation* dieAnimation = &humanInjured_;
		if (unit.isInfected()) {
			++zombiesKilled_;
			dieAnimation = &zombieInjured_;
		}

		activateFirstFreeSlot(graphicAnimations_, unit.getPosition(), unit.getDirection(), *dieAnimation);
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
		auto entry = ZombieEntry(zombieEntry_.getDeepChildEntry("settings map").getString());
		entry = entry.getDeepChildEntry("map objects object");
		while (entry.hasData()) {
			std::string geom(entry.getChildEntry("geom").getString());
			if (entry.isAttributeEqual("type", "building")) {
				auto v = loadPolygon(geom);
				Building* b = buildings_.emplaceBack(v[0], v[1], v[2], v[3], wall_, wall_, wall_);
				engine_.add(b);
				b->setActive(true);
				b->setAwake(true);
			} else if (entry.isAttributeEqual("type", "water")) {
				auto triangle = loadPolygon(geom);
				water_.addTriangle(triangle[0], triangle[1], triangle[2]);
			} else if (entry.isAttributeEqual("type", "grass")) {
				auto triangle = loadPolygon(geom);
				terrain_.addGrass(triangle[0], triangle[1], triangle[2]);
			} else if (entry.isAttributeEqual("type", "tilepoint")) {
				terrain_.addRoad(entry);
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

	float ZombieGame::getMeanFrameTime() const {
		return meanFrameTime_;
	}

} // Namespace zombie.
