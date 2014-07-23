#include "zombiebehavior.h"
#include "auxiliary.h"
#include "unit.h"

#include <list>

#include <cassert>

namespace zombie {

	ZombieBehavior::ZombieBehavior(Unit* unit) : Player(unit) {
		findNewTargetTime_ = random() * 3;
		timeToUpdateAngleDirection_ = random() * 1;
		targetAngle_ = random() * PI * 2;
		unit_ = unit;
		assert(unit_); // Null not allowed.
	}

	ZombieBehavior::~ZombieBehavior() {
		// Important to remove callback!
		targetConnection_.disconnect();
	}

	void ZombieBehavior::updatePhysics(float time, float deltaTime) {
		Input input;

		if (time > findNewTargetTime_) {
			findNewTargetTime_ = random() * 3 + time;

			MovingObject* target = findUninfectedTarget(unit_->getPosition(), unit_->getVisibleObjects());

			if (target != nullptr) {
				// Important to disconnect last connection, else risk of heap error.
				targetConnection_.disconnect();
				targetConnection_ = target->addUpdateHandler(std::bind(&ZombieBehavior::targetUpdate, this, std::placeholders::_1));
				targetUpdate(target);
			}
		}

		if (time > timeToUpdateAngleDirection_) {
			timeToUpdateAngleDirection_ = random() * 1 + time;

			// Has a target?
			if (targetConnection_.connected()) {
				Position dir = targetPosition_ - unit_->getPosition();
				targetAngle_ = std::atan2(dir.y, dir.x);
				forward_ = true;

				double distSquared = (unit_->getPosition() - unit_->getPosition()).LengthSquared();
				// Target is in range?
				if (distSquared < unit_->getWeapon()->range() * unit_->getWeapon()->range()) {
					// Attack!
					input.shoot_ = true;
				}
			} else {
				targetAngle_ += (random() - 0.5) * 2 * PI * 2 * 0.1;
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
		unit_->updatePhysics(time, deltaTime, input);
	}

	MovingObject* ZombieBehavior::getMovingObject() const {
		return unit_;
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

	void ZombieBehavior::targetUpdate(MovingObject* mOb) {
		targetPosition_ = mOb->getPosition();
	}

} // Namespace zombie.
