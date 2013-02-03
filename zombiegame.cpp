#include "zombiegame.h"

#include "inputkeyboard.h"

#include "physicalengine.h"
#include "unit.h"
#include "building.h"
#include "bullet.h"

#include "input.h"
#include "task.h"

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

		HumanPlayerPtr humanPlayer(new InputKeyboard(SDLK_UP,SDLK_DOWN,SDLK_LEFT,SDLK_RIGHT,SDLK_SPACE,SDLK_r));
		humanPlayers_.push_back(PairHumanUnit(humanPlayer,UnitPtr()));

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
		if (started_) {
			double deltaTime = msDeltaTime/1000.0;
			
			// Calculate all local ai:s input.
			for (auto& pair : aiPlayers_) {
				AiPlayerPtr& aiPlayer = pair.first;					
				UnitPtr& unit = pair.second;
				std::vector<UnitPtr> unitsInView = calculateUnitsInView(unit);
				aiPlayer->calculateInput(unit,unitsInView,time_);
			}
			
			updateGameLogic(time_,deltaTime);

			for (PairHumanUnit& pair : humanPlayers_) {
				UnitPtr& unit = pair.second;
				unit->updatePhysics(time_, deltaTime,pair.first->currentInput());
			}

			for (PairAiUnit& pair : aiPlayers_) {
				UnitPtr& unit = pair.second;
				unit->updatePhysics(time_, deltaTime,pair.first->currentInput());
			}

			// Update physics.
			physicalEngine_->update(deltaTime);
			
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

	void ZombieGame::addHuman(UnitPtr unitPtr) {
		taskManager_->add(new HumanAnimation(unitPtr));
		physicalEngine_->add(unitPtr);
		units_[unitPtr->id()] = unitPtr;
		humanPlayers_[0].second = unitPtr;
		localUnitId_ = unitPtr->id();
	}

	void ZombieGame::addNewAi(UnitPtr unitPtr) {
		taskManager_->add(new HumanAnimation(unitPtr));
		physicalEngine_->add(unitPtr);
		units_[unitPtr->id()] = unitPtr;
		AiPlayerPtr aiPlayer(new AiPlayer());
		aiPlayers_.push_back(PairAiUnit(aiPlayer,unitPtr));
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
			BuildingPtr building(new Building(corners,++unitId_));
			buildings_.push_back(building);

			taskManager_->add(new DrawBuildning(building));
			physicalEngine_->add(building);
			//std::cout << "A";
		}
		mapFile.close();
	}
	
	void ZombieGame::initGame() {
		// Add human controlled by first input device.
		UnitPtr human(new Unit(8,10,0.3,Weapon(35,0.5,8,12),false,++unitId_));		
		addHuman(human);

		// Add zombie with standard behavior.
		for (int i = 8; i < 11; i++){
			for(int j = 8; j < 11; j++) {				
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

	void ZombieGame::updateGameLogic(double time, double deltaTime) {
	}

	std::vector<UnitPtr> ZombieGame::calculateUnitsInView(const UnitPtr& unit) {
		std::vector<UnitPtr> unitsInView;
		for (auto& pair: units_) {
			UnitPtr& distantUnit = pair.second;
			Position p = distantUnit->getPosition();
			if (unit->isPointViewable(p.x_,p.y_) && isVisible(distantUnit,unit)) {
				unitsInView.push_back(distantUnit);
			}
		}

		return unitsInView;
	}
	
	void ZombieGame::calculateWeaponShooting() {
		/*
		for (auto& pair : units_) {
			UnitPtr& unit = pair.second;
			Bullet properties;
			bool shooting = unit->shooting(properties);
			if (!unit->isDead() && shooting) {
				if (properties.speed_ < 0) {
					// Weapon with infinit velocity
					// Calculate damage and target, and send hitevent.
					doShotDamage(unit, properties);
				} else {

				}
			}
		}
		*/
	}

	void ZombieGame::doShotDamage(UnitPtr shooter, Bullet properties) {
		double step = 0.0;
		double stepLength = 0.05;
		Position dr = Position(std::cos(properties.direction_),std::sin(properties.direction_));
		Position p = properties.postion_;
		bool hit = false;
		while (step < properties.range_ && !hit) {
			p += dr*stepLength;
			step += stepLength;
			/*
			for (std::vector<Object*>::iterator it = objects_.begin(); it != objects_.end(); ++it) {
				Object* ob = *it;
				if (shooter != ob && !ob->isDead() && ob->isInside(p.x_,p.y_)) {
					WeaponHit* hitEvent = new WeaponHit(ob->id(),p);
					ob->updateHealthPoint(-properties.pointDamage_);
					eventQueue_.add(hitEvent);
					hit = true;
					break;
				}
				//std::cout << step <<"\n";
			}*/
		}		
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