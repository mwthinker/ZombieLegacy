#include "unit2d.h"
#include "animation.h"
#include "auxiliary.h"
#include "weapon2d.h"

#include <mw/sound.h>

namespace zombie {

	Unit2D::Unit2D(float mass, float radius, float life, float walkingSpeed,
		float runningSpeed, bool isInfected, const WeaponPtr& weapon, const Animation& animation, Position grip) :
		Unit(mass, radius, life, walkingSpeed, runningSpeed, isInfected, weapon), animation_(animation), grip_(grip) {
		addEventHandler(std::bind(&Unit2D::eventHandler, this, this, std::placeholders::_2));
	}

	Unit2D::Unit2D(const Unit2D& unit) : Unit(unit) {
		animation_ = unit.animation_;
		addEventHandler(std::bind(&Unit2D::eventHandler, this, this, std::placeholders::_2));
		grip_ = unit.grip_;
		die_ = unit.die_;
		hit_ = unit.hit_;
	}

	Unit2D& Unit2D::operator=(const Unit2D& unit) {
		Unit::operator=(unit);
		animation_ = unit.animation_;
		die_ = unit.die_;
		hit_ = unit.hit_;
		return *this;
	}

	void Unit2D::setDieSound(const mw::Sound& sound) {
		die_ = sound;
	}

	void Unit2D::setHitSound(const mw::Sound& sound) {
		hit_ = sound;
	}

	void Unit2D::draw(float accumulator, float timeStep, gui::WindowMatrixPtr wPtr) {
		if (isActive()) {
			// Draw body.
			float worldScale = 2 * getRadius();
			Position worldPosition = getPosition();
			float angle = getDirection();
			wPtr->useShader();
			wPtr->setColor(1, 1, 1);
			mw::Matrix44 old = wPtr->getModel();
			wPtr->setModel(old * mw::getTranslateMatrix(worldPosition.x, worldPosition.y));
			mw::Matrix44 old2 = wPtr->getModel();
			wPtr->setModel(old2 * mw::getScaleMatrix(worldScale, worldScale) * mw::getRotateMatrix(getDirection(), 0, 0, 1));
			animation_.draw(timeStep, wPtr);
			wPtr->setModel(old2);
			wPtr->setModel(old2 * mw::getRotateMatrix(getDirection(), 0, 0, 1) * mw::getScaleMatrix(grip_.x, grip_.y));
			getWeapon()->draw(timeStep, wPtr);
			wPtr->setModel(old);
		}
	}

	void Unit2D::eventHandler(Unit* unit, Unit::UnitEvent unitEvent) {
		switch (unitEvent) {
			case Unit::RUN:
				// Todo!
				break;
			case Unit::STANDSTILL:
				animation_.restart();
				break;
			case Unit::DIE:
				die_.play();
				break;
			case Unit::INJURED:
				hit_.play();
				break;
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

} // Namespace zombie.
