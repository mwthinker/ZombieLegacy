#ifndef UNIT2D_H
#define UNIT2D_H

#include "unit.h"
#include "animation.h"
#include "auxiliary.h"

#include <mw/sound.h>

namespace zombie {

	class Unit2D : public Unit {
	public:
		Unit2D(float mass, float radius, float life, float walkingSpeed,
			float runningSpeed, bool isInfected, const WeaponPtr& weapon, const Animation& animation, Position grip) :
			Unit(mass, radius, life, walkingSpeed, runningSpeed, isInfected, weapon), animation_(animation), grip_(grip) {
			addEventHandler(std::bind(&Unit2D::eventHandler, this, this, std::placeholders::_2));
		}

		Unit2D(const Unit2D& unit) : Unit(unit) {
			animation_ = unit.animation_;
			addEventHandler(std::bind(&Unit2D::eventHandler, this, this, std::placeholders::_2));
			grip_ = unit.grip_;
			die_ = unit.die_;
			hit_ = unit.hit_;
		}

		Unit2D& operator=(const Unit2D& unit) {
			Unit::operator=(unit);
			animation_ = unit.animation_;
			die_ = unit.die_;
			hit_ = unit.hit_;
			return *this;
		}

		void setDieSound(const mw::Sound& sound) {
			die_ = sound;
		}

		void setHitSound(const mw::Sound& sound) {
			hit_ = sound;
		}

		void draw(float accumulator, float timeStep) override {
			if (isActive()) {
				// Draw body.
				glPushMatrix();
				glTranslate2f(getPosition());
				glScale2f(2 * getRadius());
				glRotated(getDirection() * 180 / PI, 0, 0, 1);
				glColor3d(1, 1, 1);
				animation_.draw(timeStep);
				glPopMatrix();

				// Draw weapon.
				glPushMatrix();
				glTranslate2f(getPosition());
				glScale2f(2 * getRadius());
				glRotated(getDirection() * 180 / PI, 0, 0, 1);
				glTranslate2f(grip_);
				getWeapon()->draw();
				glPopMatrix();
			}
		}

		void eventHandler(Unit* unit, Unit::UnitEvent unitEvent) {
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
				case Unit::SHOOT:
					getWeapon()->playShotSound();
					break;
				case Unit::RELOADING:
					getWeapon()->playReloadSound();
					break;
			}
		}

	private:
		Animation animation_;
		Position grip_;
		mw::Sound die_;
		mw::Sound hit_;
	};

} // Namespace zombie.

#endif // UNIT2D_H
