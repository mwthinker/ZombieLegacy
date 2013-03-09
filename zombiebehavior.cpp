#include "zombiebehavior.h"

#include "unit.h"

#include <mw/mathvector.h>

#include <vector>

namespace zombie {

	ZombieBehavior::ZombieBehavior() {
		findNewTargetTime_ = random() * 3;
		timeToUpdateAngleDirection_ = random() * 1;
		targetAngle_ = random() * mw::PI * 2;
		forward_ = true;
	}

	ZombieBehavior::~ZombieBehavior() {
	}

	Input ZombieBehavior::calculateInput(const UnitPtr& unit, const std::vector<UnitPtr>& units, double time) {
		Input input;

		// Target is valid and dead?
		if (target_ != nullptr && target_ ->isDead()) {
			target_ = nullptr;
		}

		if (time > findNewTargetTime_) {
			findNewTargetTime_ = random() * 3 + time;
			target_ = findUninfectedTarget(unit->getPosition(), units);				
		}

		if (time > timeToUpdateAngleDirection_) {
			timeToUpdateAngleDirection_ = random() * 1 + time;

			// Has a target?
			if (target_ != nullptr) {
				mw::MathVector dir = target_->getPosition() - unit->getPosition();
				targetAngle_ = std::atan2(dir.y_,dir.x_);
				forward_ = true;

				double distSquared = (unit->getPosition() - unit->getPosition()).magnitudeSquared();
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

		double diffAngle = calculateDifferenceBetweenAngles(targetAngle_,unit->moveDirection());

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

	UnitPtr ZombieBehavior::findUninfectedTarget(Position position, const std::vector<UnitPtr>& units) const {			
		UnitPtr target(nullptr);
		double distant = 100;
		for (const UnitPtr& unit : units) {
			// Not infected?
			if (!unit->isInfected()) {
				double tmp = (position - unit->getPosition()).magnitudeSquared();					
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
