#ifndef ZOMBIE2D_H
#define ZOMBIE2D_H

#include "unit.h"
#include "animation.h"
#include "auxiliary.h"

namespace zombie {

	class Zombie2D : public Unit {
	public:
		Zombie2D(float mass, float radius, float life, float walkingSpeed, 
			float runningSpeed, const Weapon& weapon, const Animation& animation) :
			Unit(mass, radius, life, walkingSpeed, runningSpeed, true, weapon), animation_(animation) {
			addEventHandler(std::bind(&Zombie2D::eventHandler, this, this, std::placeholders::_2));
		}

		void draw(float time) override {
			// Draw body.
			glPushMatrix();
			glTranslate2f(getPosition());
			glScale2f(getRadius());
			glRotated(getDirection() * 180 / PI, 0, 0, 1);
			animation_.draw(time);
			glPopMatrix();
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

#endif // ZOMBIE2D_H
