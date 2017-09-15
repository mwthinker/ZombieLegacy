#include "unit2d.h"
#include "animation.h"
#include "auxiliary.h"
#include "weapon2d.h"
#include "missilelauncher2d.h"
#include "gun.h"

#include <mw/sound.h>

namespace zombie {

	Unit2D::Unit2D() :
		Unit(1, 1, 1, 1, 1, false, ZERO, nullptr) {
		
	}

	Unit2D::Unit2D(float mass, float radius, float life, float walkingSpeed,
		float runningSpeed, bool isInfected, const WeaponPtr& weapon, const Animation& animation, Position grip) :
		Unit(mass, radius, life, walkingSpeed, runningSpeed, isInfected, grip, weapon), animation_(animation) {
	}

	Unit2D::Unit2D(const Unit2D& unit) : Unit(unit) {
		animation_ = unit.animation_;
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

	void Unit2D::draw(float accumulator, float timeStep, const GameShader& gameShader) {
		if (isActive()) {
			// Draw body.
			float diameter = 2 * getRadius();
			float angle = getDirection();
			
			gameShader.useProgram();
			gameShader.setGlColorU(1, 1, 1);
			gameShader.setGlGlobalPositionU(getPosition());
			gameShader.setGlLocalAngleU(getDirection());
			animation_.draw(timeStep, 0, 0, diameter, diameter, gameShader);
			Position grip = getGrip();
			getWeapon()->draw(timeStep, grip.x, grip.y, gameShader);
		}
	}

	void Unit2D::eventHandler(int unitEvent) {
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

	Unit2D loadUnit(GameInterface* gameInterface, const UnitProperties& properties, bool infected) {
		std::shared_ptr<Weapon> weapon;
		if (properties.weaponProperties_.type_ == WeaponProperties::MISSILE) {
			weapon = loadMissileLauncher2D(gameInterface, properties.weaponProperties_).clone();
		} else {
			weapon = loadGun(gameInterface, properties.weaponProperties_).clone();
		}
		return Unit2D(properties.mass_, properties.radius_, properties.life_, properties.walkingSpeed_, properties.runningSpeed_, infected, weapon, properties.moveAnimation_, properties.grip_);
	}

} // Namespace zombie.
