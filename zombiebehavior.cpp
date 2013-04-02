#include "zombiebehavior.h"

#include "unit.h"

#include <mw/mathvector.h>

#include <list>

namespace zombie {

	ZombieBehavior::ZombieBehavior() {
		findNewTargetTime_ = random() * 3;
		timeToUpdateAngleDirection_ = random() * 1;
		targetAngle_ = random() * mw::PI * 2;
		forward_ = true;
		target_ = nullptr;
	}

	ZombieBehavior::~ZombieBehavior() {
	}

	Input ZombieBehavior::calculateInput(const Unit* unit, double time) {
		Input input;

		if (!connectionToTaget_.connected()) {
			target_ = nullptr;
		}

		// Target is valid and dead?
		if (target_ != nullptr && target_ ->isDead()) {
			target_ = nullptr;
		}

		if (time > findNewTargetTime_) {
			findNewTargetTime_ = random() * 3 + time;
			connectionToTaget_.disconnect();
			
			target_ = findUninfectedTarget(unit->getPosition(), unit->getVisibleObjects());
			if (target_ != nullptr) {
				connectionToTaget_ = target_->addExistHandler();
			}
		}

		if (time > timeToUpdateAngleDirection_) {
			timeToUpdateAngleDirection_ = random() * 1 + time;

			// Has a target?
			if (target_ != nullptr) {
				Position dir = target_->getPosition() - unit->getPosition();
				targetAngle_ = std::atan2(dir.y,dir.x);
				forward_ = true;

				double distSquared = (unit->getPosition() - unit->getPosition()).LengthSquared();
				// Target is in range?
				if (distSquared < unit->getWeapon().range()*unit->getWeapon().range()) {
					// Attack!
					input.shoot_ = true;
				}
			} else {
				targetAngle_ += (random()-0.5)*2 * mw::PI * 2 * 0.1;
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

	void ZombieBehavior::unitEventHandler(Unit::UnitEvent unitEvent) {
	}

} // Namespace zombie.
