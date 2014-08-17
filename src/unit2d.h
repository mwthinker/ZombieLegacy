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
				float worldScale = 2 * getRadius();
				Position worldPosition = getPosition();
				float angle = getDirection() * 180 / PI;

				glColor3d(1, 1, 1);
				glPushMatrix();
				glTranslate2f(worldPosition);
				glPushMatrix();
				glScale2f(worldScale);
				glRotatef(angle);
				animation_.draw(timeStep);
				glPopMatrix();
				glRotatef(angle);
				glTranslate2f(grip_);
				getWeapon()->draw(timeStep);
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
