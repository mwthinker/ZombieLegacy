#include "zombiebehavior.h"
#include "auxiliary.h"
#include "unit.h"

#include <list>

#include <cassert>

namespace zombie {

	ZombieBehavior::ZombieBehavior(Unit* unit) {
		findNewTargetTime_ = random() * 3;
		timeToUpdateAngleDirection_ = random() * 1;
		//targetAngle_ = random() * PI * 2;
		unit_ = unit;
		assert(unit_); // Null not allowed.
		targetAngle_ = unit->getDirection();
		target_ = nullptr;
	}

	ZombieBehavior::~ZombieBehavior() {
	}

	void ZombieBehavior::updateInput(float time, float deltaTime) {
		Input input;

		if (time > findNewTargetTime_) {
			findNewTargetTime_ = random() * 3 + time;

			target_ = findUninfectedTarget(unit_->getPosition(), unit_->getVisibleObjects());
		}

		if (time > timeToUpdateAngleDirection_) {
			timeToUpdateAngleDirection_ = random() * 1 + time;

			// Has a target?
			if (target_ != nullptr && target_->isActive() && !target_->isDead()) {
				Position targetPosition = target_->getPosition();
				Position position = unit_->getPosition();
				Position dir = targetPosition - position;
				targetAngle_ = std::atan2(dir.y, dir.x);
				forward_ = true;

				double distSquared = dir.LengthSquared();
				// Target is in range?
				if (distSquared < unit_->getWeapon()->getRange() * unit_->getWeapon()->getRange()) {
					// Attack!
					input.shoot_ = true;
				}
			} else {
				targetAngle_ +=  (random() - 0.5f) * 2 * PI * 2 * 0.05f;
				forward_ = random() > 0.25;
			}
		}

		double diffAngle = calculateDifferenceBetweenAngles(targetAngle_, unit_->getDirection());

		// Turn?
		if (std::abs(diffAngle) > 0.1) {
			if (diffAngle > 0) {
				input.turnLeft_ = true;
			} else {
				input.turnRight_ = true;
			}
		}

		input.forward_ = forward_;
		unit_->setInput(input);
	}

	MovingObject* ZombieBehavior::getMovingObject() const {
		return unit_;
	}

	MovingObject* ZombieBehavior::findUninfectedTarget(Position position, const std::list<MovingObject*>& units) const {
		MovingObject* target(nullptr);

		float distant = 100;
		for (MovingObject* unit : units) {
			// Not infected?
			if (!unit->isInfected()) {
				float tmp = (position - unit->getPosition()).LengthSquared();
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
