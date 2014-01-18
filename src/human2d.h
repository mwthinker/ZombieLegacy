#ifndef HUMAN2D_H
#define HUMAN2D_H

#include "unit.h"
#include "animation.h"
#include "auxiliary.h"

namespace zombie {

	class Human2D : public Unit {
	public:
		Human2D(b2World* world, const State& state, float mass, float radius, 
			float life, float walkingSpeed, float runningSpeed, const Weapon& weapon,
			const Animation& animation) :
			Unit(world, state, mass, radius, life, walkingSpeed, runningSpeed, false, weapon), animation_(animation) {

		}

		void draw(float time) override {
			// Draw body
			glPushMatrix();
			glTranslate2f(getPosition());
			glScale2f(getRadius());
			glRotated(getDirection() * 180 / PI, 0, 0, 1);
			animation_.draw(time);
			glPopMatrix();
		}

		void eventHandler(Unit::UnitEvent unitEvent) {
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

#endif // HUMAN2D_H
