#include "survivorbehavior.h"
#include "aibehavior.h"
#include "order.h"
#include "typedefs.h"
#include "unit.h"
#include "auxiliary.h"

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