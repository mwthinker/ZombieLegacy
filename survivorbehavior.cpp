#include "survivorbehavior.h"

#include "unit.h"

#include <mw/mathvector.h>

#include <vector>

namespace zombie {

	SurvivorBehavior::SurvivorBehavior() {
		findNewTargetTime_ = random() * 3;
		timeToUpdateAngleDirection_ = random() * 1;
		targetAngle_ = random() * mw::PI * 2;
		forward_ = true;
		backward_ = false;
		target_ = nullptr;
	}

	SurvivorBehavior::~SurvivorBehavior() {
	}

	Input SurvivorBehavior::calculateInput(const Unit* unit, double time) {
		Input input;
		// extract information from memory
		// analyze situation - enviroment and units when neccesarry
		// make decision on what activity to perform
		Activity a;
		auto units = unit->getVisibleObjects();

		if (units.size() > 0) {
			a = FLEE;
		} else {
			a = PANIC;
		}

		
		return activityToInput(a, unit, time);
	}

	Input SurvivorBehavior::activityToInput(Activity a, const Unit* unit, double time) {
		if(s_.size() > 0) {
			// Do planned subroutines		
		}
		Input input;
		switch(a) {
			case PANIC:
				input.turnLeft_ = true;
				input.forward_ = false;
			break;
			case FLEE:
				input.forward_ = true;
			break;

		}
		return input;
	}

	Unit* SurvivorBehavior::findUninfectedTarget(Position position, const std::vector<Unit*>& units) const {			
		Unit* target(nullptr);
		double distant = 100;
		for (Unit* unit : units) {
			// Not infected?
			if (unit->isInfected()) {
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



// OLD CODE
/*

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
				Position dir = target_->getPosition() - unit->getPosition();
				targetAngle_ = std::atan2(dir.y,dir.x);
				forward_ = true;

				double distSquared = (unit->getPosition() - unit->getPosition()).LengthSquared();
				// Target is in range?
				if (distSquared < unit->getWeapon().range()*unit->getWeapon().range()) {
					// Attack!
					if((targetAngle_ - unit->getAngle())*(targetAngle_ - unit->getAngle()) < 0.05 ) {
						input.shoot_ = true;
						forward_ = false;
						backward_ = random() > 0.50;
					} else {
						backward_ = false;
					}
				} else {
					backward_ = false;
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
		input.backward_ = backward_;
		*/