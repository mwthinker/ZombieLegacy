#include "zombiegame.h"

#include "inputkeyboard.h"

#include "physicalengine.h"
#include "unit.h"
#include "building.h"
#include "bullet.h"

#include "input.h"
#include "task.h"
#include "taskmanager.h"

#include <SDL.h>
#include <memory> //std::shared_ptr
#include <vector>

#include <string>
#include <fstream>

namespace zombie {

    ZombieGame::ZombieGame() {
		taskManager_ = new TaskManager();
		physicalEngine_ = new PhysicalEngine();
		
		started_ = false;
		time_ = 0.0;
		unitId_ = 0;
		timeToUpdateView_ = 0.25; // Seconds in which the view is assumed to be constant in order to 
		                          // speed up calculations.
		indexAiPlayer_ = 0;

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

    void ZombieGame::physicUpdate(Uint32 msDeltaTime) {
		// Game is started?
		if (started_) {
			double deltaTime = msDeltaTime/1000.0;
			
			// (deltaTime) must always be larger than (timeToUpdateView_).
			int stopIndex = aiPlayers_.size() * (deltaTime/timeToUpdateView_);
			// In case size has changed.
			indexAiPlayer_ = indexAiPlayer_ % aiPlayers_.size();
			// Update some of the ai:s view.
			while (indexAiPlayer_ != stopIndex) {
				AiPlayerPtr& aiPlayer = aiPlayers_[indexAiPlayer_].first;					
				UnitPtr& unit = aiPlayers_[indexAiPlayer_].second;
				std::vector<UnitPtr> unitsInView = calculateUnitsInView(unit);
				//aiPlayer->updateUnitsInView(unitsInView);
				indexAiPlayer_ = (indexAiPlayer_ + 1) % aiPlayers_.size();
			}

			// Calculate all local ai:s input.
			for (auto& pair : aiPlayers_) {
				AiPlayerPtr& aiPlayer = pair.first;					
				UnitPtr& unit = pair.second;
				aiPlayer->calculateInput(unit,time_);
			}
			
			// Update shooting and reloading.

			// Update all units.
			for (PairPlayerUnit& pair : players_) {
				UnitPtr& unit = pair.second;
				unit->updatePhysics(time_, deltaTime,pair.first->currentInput());

				Bullet bullet;
				// Alive? And shooting?
				if (!unit->isDead() && unit->pollShot(bullet)) {
					doShotDamage(unit, bullet);
				}
			}		

			// Update the objects physics interactions.
			physicalEngine_->update(deltaTime);
			
			// Move the time ahead.
			time_ += deltaTime;
		}
	}

	void ZombieGame::graphicUpdate(Uint32 msDeltaTime) {
		// Game is started?
		if (started_) {			
			taskManager_->update(msDeltaTime/1000.0);
		} else {
			taskManager_->update(0.0);
		}
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
		physicalEngine_->add(unitPtr);
		humanPlayers_.push_back(PairHumanUnit(human,unitPtr));
		players_.push_back(PairPlayerUnit(human,unitPtr));
	}

	void ZombieGame::addNewAi(UnitPtr unitPtr) {
		taskManager_->add(new HumanAnimation(unitPtr));
		physicalEngine_->add(unitPtr);
		AiPlayerPtr aiPlayer(new AiPlayer());
		aiPlayers_.push_back(PairAiUnit(aiPlayer,unitPtr));
		players_.push_back(PairPlayerUnit(aiPlayer,unitPtr));
	}	

	void ZombieGame::loadMap(std::string filename) {
		std::fstream mapFile(filename.c_str(),std::fstream::in);
		
		if (mapFile.good()) {
			mapFile >> width_ >> height_;
		}
		
		while (mapFile.good()) {
			std::vector<Position> corners;
			while (mapFile.good()) {
				Position p;
				mapFile >> p.x_ >> p.y_;
				if (p.x_ < 0 || p.y_ < 0) {
					break;
				} else {
					corners.push_back(p);
				}
			}
			BuildingPtr building = BuildingPtr(new Building(corners,++unitId_));
			buildings_.push_back(building);

			taskManager_->add(new DrawBuildning(building));
			physicalEngine_->add(building);
			//std::cout << "A";
		}
		std::vector<Position> corners;
		corners.push_back(Position(0,0));
		corners.push_back(Position(0,width_));
		corners.push_back(Position(height_,width_));
		corners.push_back(Position(height_,0));
		
		worldBorder_ = BuildingPtr(new Building(corners,++unitId_));
		taskManager_->add(new DrawBuildning(worldBorder_));
		physicalEngine_->add(worldBorder_);
		
		mapFile.close();
	}
	
	void ZombieGame::initGame() {
		// Add human controlled by first input device.
		UnitPtr human(new Unit(2,2,0.3,Weapon(35,0.5,8,12),false,++unitId_));
		HumanPlayerPtr humanPlayer(new InputKeyboard(SDLK_UP,SDLK_DOWN,SDLK_LEFT,SDLK_RIGHT,SDLK_SPACE,SDLK_r));
		addHuman(humanPlayer,human);

		// Add zombie with standard behavior.
		for (int i = 1; i < 10; i++){
			for(int j = 1; j < 10; j++) {
				UnitPtr zombie(new Unit(8+i,10+j,0.3*i+j,Weapon(35,0.5,8,12),false,++unitId_));
				addNewAi(zombie);
			}
		}

		loadMap("buildings.txt");
	}

	// ZombieGame

	double ZombieGame::getWidth() const {
		return 400.0;
	}

	double ZombieGame::getHeight() const {
		return 400.0;
	}
	
	std::vector<UnitPtr> ZombieGame::calculateUnitsInView(const UnitPtr& unit) {
		std::vector<UnitPtr> unitsInView;
		for (auto& pair: players_) {
			UnitPtr& distantUnit = pair.second;
			Position p = distantUnit->getPosition();
			if (unit->isPointViewable(p.x_,p.y_) && isVisible(distantUnit,unit)) {
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
		while (step < bullet.range_ && !hit) {
			p += dr*stepLength;
			step += stepLength;
			
			for (BuildingPtr& building : buildings_) {
				// Hits a building?
				if (worldBorder_ != building && building->isInside(p.x_,p.y_)) {
					hit = true;
					break;
				}

				for (PairPlayerUnit& playerUnit : players_) {
					UnitPtr& unit = playerUnit.second;

					// Not hitting itself? And target not dead? And bullet inside target?
					if (shooter != unit && !unit->isDead() && unit->isInside(p.x_,p.y_)) {
						unit->updateHealthPoint(-bullet.damage_);
						if(unit->isDead()){
							taskManager_->add(new Death(p.x_,p.y_,time_));
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
		}

		//taskManager_->add(new Shot(p.x_,p.y_,time_));
	}	

	bool ZombieGame::isVisible(UnitPtr unitToBeSeen, UnitPtr unitThatSees) const {
		Position dr = unitToBeSeen->getPosition() - unitThatSees->getPosition();
		double length = dr.magnitude();
		double stepLength = 0.1;
		double step = 0.0;
		dr = dr.normalize();
		
		Position p = unitThatSees->getPosition();
		while (step < length) {
			p += dr*stepLength;
			step += stepLength;
			for (const BuildingPtr& building : buildings_) {
				if (building->isInside(p.x_,p.y_)) {
					return false;
				}
			}
		}
		return true;
	}	

} // namespace zombie