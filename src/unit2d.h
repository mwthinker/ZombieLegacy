#ifndef UNIT2D_H
#define UNIT2D_H

#include "unit.h"
#include "animation.h"
#include "auxiliary.h"

namespace zombie {

	class Unit2D : public Unit {
	public:
		Unit2D(float mass, float radius, float life, float walkingSpeed,
			float runningSpeed, bool isInfected, const WeaponPtr& weapon, const Animation& animation) :
			Unit(mass, radius, life, walkingSpeed, runningSpeed, isInfected, weapon), animation_(animation) {
			addEventHandler(std::bind(&Unit2D::eventHandler, this, this, std::placeholders::_2));
		}

		Unit2D(const Unit2D& unit) : Unit(unit) {
			animation_ = unit.animation_;
			addEventHandler(std::bind(&Unit2D::eventHandler, this, this, std::placeholders::_2));
		}

		Unit2D& operator=(const Unit2D& unit) {
			animation_ = unit.animation_;
		}

		void draw(float time) override {
			if (isActive()) {
				// Draw body.
				glPushMatrix();
				glTranslate2f(getPosition());
				glScale2f(2 * getRadius());
				glRotated(getDirection() * 180 / PI, 0, 0, 1);
				getWeapon()->draw();
				animation_.draw(time);
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
				default:
					break;
			}
		}

	private:
		Animation animation_;
	};

} // Namespace zombie.

#endif // UNIT2D_H
