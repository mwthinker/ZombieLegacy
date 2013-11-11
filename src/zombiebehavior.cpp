#include "zombiebehavior.h"
#include "auxiliary.h"
#include "movingobject.h"

#include <list>

namespace zombie {

	ZombieBehavior::ZombieBehavior() {
		findNewTargetTime_ = random() * 3;
		timeToUpdateAngleDirection_ = random() * 1;
		targetAngle_ = random() * PI * 2;
	}

	ZombieBehavior::~ZombieBehavior() {
	}

	Input ZombieBehavior::calculateInput(const Unit* unit, double time) {
		Input input;

		MovingObject* target = nullptr;

		if (time > findNewTargetTime_) {
			findNewTargetTime_ = random() * 3 + time;
			
			target = findUninfectedTarget(unit->getPosition(), unit->getVisibleObjects());
		}

		if (time > timeToUpdateAngleDirection_) {
			timeToUpdateAngleDirection_ = random() * 1 + time;

			// Has a target?
			if (target != nullptr) {
				Position dir = target->getPosition() - unit->getPosition();
				targetAngle_ = std::atan2(dir.y,dir.x);
				forward_ = true;

				double distSquared = (unit->getPosition() - unit->getPosition()).LengthSquared();
				// Target is in range?
				if (distSquared < unit->getWeapon().range()*unit->getWeapon().range()) {
					// Attack!
					input.shoot_ = true;
				}
			} else {
				targetAngle_ += (random()-0.5)*2 * PI * 2 * 0.1;
				forward_ = random() > 0.25;
			}
		}			

		double diffAngle = calculateDifferenceBetweenAngles(targetAngle_,unit->getDirection());

		// Turn?
		if (std::abs(diffAngle) > 0.1 ) {
			if (diffAngle > 0) {
				input.turnLeft_ = true;
			} else {
				input.turnRight_ = true;
			}
		}

		input.forward_ = forward_;

		return input;
	}

	MovingObject* ZombieBehavior::findUninfectedTarget(Position position, const std::list<MovingObject*>& units) const {			
		MovingObject* target(nullptr);
		double distant = 100;
		for (MovingObject* unit : units) {
			// Not infected?
			if (!unit->isInfected()) {
				double tmp = (position - unit->getPosition()).LengthSquared();					
				// Closer?
				if (tmp < distant) {
					target = unit;
					distant = tmp;
				}
			}
		}
		return target;
	}

} // Namespace zombie.
