#include "zombiegame.h"

#include "typedefs.h"
#include "inputkeyboard.h"
#include "physicalengine.h"
#include "unit.h"
#include "building.h"
#include "bullet.h"
#include "map.h"
#include "tile.h"
#include "input.h"
#include "task.h"
#include "quadtree.h"
#include "taskmanager.h"
#include "aiplayer.h"
#include "survivaltimer.h"
#include "graphictask.h"

#include <SDL.h>
#include <memory>
#include <vector>
 
#include <string>
#include <fstream>
#include <gl/GLU.h>

namespace zombie {

    ZombieGame::ZombieGame(int width, int height) {
		taskManager_ = new TaskManager();
		
		updateSize(width,height);
		scale_ = 1.0;
		
		started_ = false;
		time_ = 0.0;
		timeToUpdateView_ = 0.25; // Seconds in which the view is assumed to be constant in order to 
		                          // speed up calculations.

		timeToUpdateSpawn_ = 0.5; // Time between spawns and unit clean ups
		timeSinceSpawn_ = 0.0;
		indexAiPlayer_ = 0;
		unitLevel_ = 100;
		innerSpawnRadius_ = 10;
		outerSpawnRadius_ = 20;

		timeStep_ = 0.05;  // Fix time step for physics update.
		accumulator_ = 0.0;      // Time accumulator.

		initGame();
	}

    ZombieGame::~ZombieGame() {
		delete physicalEngine_;
		delete taskManager_;
	}
    
	void ZombieGame::startGame() {
		// Start game!
		started_ = true;
	}

    void ZombieGame::updatePhysics(double timeStep) {
		// Game is started?
		if (started_) {
			// Spawn and clean up units
			if (timeSinceSpawn_ > timeToUpdateSpawn_) {
				spawnAndCleanUpUnits();
				timeSinceSpawn_ = 0;
			}
			timeSinceSpawn_ += timeStep;

			// (deltaTime) must always be larger than (timeToUpdateView_).
			int nbrOfUnitsToUpdateViewOn = (int) (aiPlayers_.size() * (timeStep/timeToUpdateView_));
			// Update some of the ai:s view.
			for (int i = 0; i < nbrOfUnitsToUpdateViewOn; ++i) {
				indexAiPlayer_ = (indexAiPlayer_ + 1) % aiPlayers_.size();
				AiPlayerPtr& aiPlayer = aiPlayers_[indexAiPlayer_].first;
				UnitPtr& unit = aiPlayers_[indexAiPlayer_].second;
				std::vector<UnitPtr> unitsInView = calculateUnitsInView(unit);
				aiPlayer->updateUnitsInView(unitsInView);
			}

			// Calculate all local ai:s input.
			for (auto& pair : aiPlayers_) {
				AiPlayerPtr& aiPlayer = pair.first;
				UnitPtr& unit = pair.second;
				aiPlayer->calculateInput(unit,time_);				
			}
			
			// Update all units.
			for (PairPlayerUnit& pair : players_) {
				UnitPtr& unit = pair.second;
				unit->updatePhysics(time_, timeStep,pair.first->currentInput());

				Bullet bullet;
				// Alive? And shooting?
				if (!unit->isDead() && unit->pollShot(bullet)) {
					doShotDamage(unit, bullet);
				}
			}		

			// Update the objects physics interactions.
			physicalEngine_->update(timeStep);
			
			// Move the time ahead.
			time_ += timeStep;
		}
	}

	void ZombieGame::spawnAndCleanUpUnits() {
		Position center = humanPlayers_[0].second->getPosition();
		

		// delete zombies outside of perimiter ***************
		UnitPtr temp;
		/*std::remove_if(players_.begin(), players_.end(),[] (const PairPlayerUnit& pair) {
			Position unitPos = pair.second->getPosition();
			return pair.second;
		});*/

		for (auto it = players_.begin(); it != players_.end(); it++) {
			Position unitPos = it->second->getPosition();
			if((unitPos-center)*(unitPos-center) > outerSpawnRadius_*outerSpawnRadius_) {
				// REMOVE UNIT
				it->second->setIsDead();
				temp = it->second;
				players_.erase(it);
				break;
			}
		}

		for (auto it = aiPlayers_.begin(); it != aiPlayers_.end(); it++) {
			if(it->second == temp) {
				aiPlayers_.erase(it);
				break;
			}
		}

		// spawn new units ************************************
		int nbrOfZombies = aiPlayers_.size();
		while (nbrOfZombies < unitLevel_) {
			// INSERT ZOMBIE
			Position p = map_.generateSpawnPosition(humanPlayers_[0].second->getPosition(),innerSpawnRadius_,outerSpawnRadius_);
			UnitPtr zombie(new Unit(p.x_,p.y_,0.3,Weapon(35,0.5,1,12),true));
			addNewAi(zombie);			
			nbrOfZombies++;
		}
	}

	void ZombieGame::update(double deltaTime) {
		// DeltaTime to big?
		if (deltaTime > 0.250) {
			// To avoid spiral of death.
			deltaTime = 0.250;
		}

		accumulator_ += deltaTime;
		while (accumulator_ >= timeStep_) {
			accumulator_ -= timeStep_;		
			updatePhysics(timeStep_);
		}

		// Draw map centered around first humna player.
		UnitPtr unit = humanPlayers_[0].second;
		glPushMatrix();
		
		Position p = unit->getPosition();
		viewPosition_ += (unit->getPosition() - viewPosition_) * deltaTime;

		// 3D?
		if (graphic3D_) {
			glEnable (GL_DEPTH_TEST);
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glOrtho(-10, 10, -10, 10, -10, 100);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			gluLookAt(0,-2, 1,
				0,0,0,
				0,1,0);
		} else { // 2D!
			glTranslated(0.5, 0.5, 0);
			glScaled(1.0/50,1.0/50,1); // Is to fit the box drawn where x=[0,1] and y=[0,1].
			
			drawCircle(0,0,0.5,20,false);
			glScaled(scale_,scale_,1); // Is to fit the box drawn where x=[0,1] and y=[0,1].
			drawCircle(0,0,0.5,20,false);
		}		

		glTranslated(-viewPosition_.x_,-viewPosition_.y_,0.0);
		//glTranslated(-p.x_,-p.y_,0.0);
		
		// Game is started?
		if (started_) {
			taskManager_->update(deltaTime);
		} else {
			taskManager_->update(0.0);
		}
		glPopMatrix();
	}

	void ZombieGame::eventUpdate(const SDL_Event& windowEvent) {
		// Game is started?
		if (started_) {
			// Update all human input.
			for (auto& pair : humanPlayers_) {
				HumanPlayerPtr& humanPlayer = pair.first;
				humanPlayer->eventUpdate(windowEvent);
			}
		}
	}

	void ZombieGame::addHuman(HumanPlayerPtr human, UnitPtr unitPtr) {
		taskManager_->add(new HumanAnimation(unitPtr));
		taskManager_->add(new HumanStatus(unitPtr,HumanStatus::ONE));
		physicalEngine_->add(unitPtr);
		humanPlayers_.push_back(PairHumanUnit(human,unitPtr));
		players_.push_back(PairPlayerUnit(human,unitPtr));
	}

	void ZombieGame::addNewAi(UnitPtr unitPtr) {
		if (unitPtr->isInfected()) {
			taskManager_->add(new ZombieAnimation(unitPtr));
		} else {
			taskManager_->add(new HumanAnimation(unitPtr));
		}
		physicalEngine_->add(unitPtr);
		AiPlayerPtr aiPlayer(new AiPlayer());
		aiPlayers_.push_back(PairAiUnit(aiPlayer,unitPtr));
		players_.push_back(PairPlayerUnit(aiPlayer,unitPtr));
	}
	
	void ZombieGame::normalizeBuildings() {
		for (BuildingPtr building : buildings_) {

		}
	}

	void ZombieGame::initGame() {
		graphic3D_ = false;
		
		//map_ = loadMapInfo("housesFME.mif","roadsFME.mif", 1);
		map_ = loadMapInfo("housesFME.mif","roadsFME.mif", 1);
		//Map mapTile1 = loadTile("housesFME.mif","roadsFME.mif",100);
		
		//map_ = createTiledMap(mapTile1);

		physicalEngine_ = new PhysicalEngine(map_.minX(),map_.minY(),map_.width(),map_.height());
		buildings_ = Quadtree<BuildingPtr>(map_.minX(),map_.minY(),map_.width(),map_.height(),4);

		taskManager_->add(new MapDraw(map_));
		taskManager_->add(new RoadDraw(map_));

		auto buildings = map_.getBuildings();

		for (BuildingPtr building : buildings) {
			if (graphic3D_) {
			//taskManager_->add(new Buildning3DTask(building));
			} else {
				//taskManager_->add(new DrawBuildning(building),3);
				taskManager_->add(new DrawFake3DBuildning(building));
			}			
			physicalEngine_->add(building);
			buildings_.add(building,building->getPosition().x_,building->getPosition().y_,building->getRadius());
		}
		
		// Add human controlled by first input device.
		Position position = map_.generateSpawnPosition();
		UnitPtr human(new Unit(position.x_,position.y_,0.3,Weapon(35,0.2,8,12),false));
		viewPosition_ = human->getPosition();

		HumanPlayerPtr humanPlayer(new InputKeyboard(SDLK_UP,SDLK_DOWN,SDLK_LEFT,SDLK_RIGHT,SDLK_SPACE,SDLK_r,SDLK_LSHIFT));
		addHuman(humanPlayer,human);

		// Add zombie with standard behavior.
		for (int i = 0; i < 40; i++) {
			Position spawn = map_.generateSpawnPosition(human->getPosition(),innerSpawnRadius_,outerSpawnRadius_);
			UnitPtr zombie(new Unit(spawn.x_,spawn.y_,0.3,Weapon(35,0.5,1,12),true));
			addNewAi(zombie);
		}
		
		for (int i = 0; i < 5; i++) {
			Position spawn = map_.generateSpawnPosition(human->getPosition(),1,innerSpawnRadius_);
			UnitPtr survivor(new Unit(spawn.x_,spawn.x_,spawn.x_,Weapon(35,0.5,8,12),false));
			addNewAi(survivor);
		}	
	}

	void ZombieGame::zoom(double scale) {
		scale_ *= scale;
	}
	
	void ZombieGame::updateSize(int width, int height) {
		Task::width = width;
		Task::height = height;
	}

	std::vector<UnitPtr> ZombieGame::calculateUnitsInView(const UnitPtr& unit) {
		std::vector<UnitPtr> unitsInView;
		for (auto& pair: players_) {
			UnitPtr& distantUnit = pair.second;
			Position p = distantUnit->getPosition();
			
			if (unit != distantUnit && 
				( unit->isInsideSmalViewDistance(p.x_,p.y_)) || (unit->isPointViewable(p.x_,p.y_) && isVisible(distantUnit,unit)) ) {
				unitsInView.push_back(distantUnit);
			}
		}

		return unitsInView;
	}

	void ZombieGame::doShotDamage(UnitPtr shooter, const Bullet& bullet) {
		double step = 0.0;
		double stepLength = 0.05;
		Position dr = Position(std::cos(bullet.direction_),std::sin(bullet.direction_));
		Position p = bullet.postion_;
		bool hit = false;
		
		auto buildings = buildings_.getObjectsAt(bullet.postion_.x_,bullet.postion_.y_,bullet.range_);

		while (step < bullet.range_ && !hit) {
			p += dr*stepLength;
			step += stepLength;
			
			for (BuildingPtr& building : buildings) {
				// Hits a building?
				if (building->isInside(p.x_,p.y_)) {
					hit = true;
					break;
				}
			}

			if (hit) {
				break;
			}

			for (PairPlayerUnit& playerUnit : players_) {
				UnitPtr& unit = playerUnit.second;

				// Not hitting itself? And target not dead? And bullet inside target?
				if (shooter != unit && !unit->isDead() && unit->isInside(p.x_,p.y_)) {
					unit->updateHealthPoint(-bullet.damage_);
					if(unit->isDead()){
						//taskManager_->add(new Death(p.x_,p.y_,time_),2);
						taskManager_->add(new BloodStain(p.x_,p.y_,time_));
						taskManager_->add(new Blood(p.x_,p.y_,time_));
						
					} else {
						taskManager_->add(new BloodSplash(p.x_,p.y_,time_));
					}
					hit = true;
					break;
				}
			}

			if (hit) {
				break;
			}
		}

		//taskManager_->add(new Shot(p.x_,p.y_,time_));
	}	

	bool ZombieGame::isVisible(UnitPtr unitToBeSeen, UnitPtr unitThatSees) const {
		Position dr = unitToBeSeen->getPosition() - unitThatSees->getPosition();
		double length = dr.magnitude();
		double stepLength = 1;
		double step = 0.0;
		dr = dr.normalize();
				
		Position p = unitThatSees->getPosition();
		
		auto buildings = buildings_.getObjectsAt(p.x_,p.y_,length*2);
		while (step < length) {
			p += dr*stepLength;
			step += stepLength;
			for (const BuildingPtr& building : buildings) {
				if (building->isInside(p.x_,p.y_)) {
					return false;
				}
			}
		}
		return true;
	}
} // namespace zombie