#include "survivorbehavior.h"
#include "unit.h"
#include "auxiliary.h"

namespace zombie {

	SurvivorBehavior::SurvivorBehavior() {
		findNewTargetTime_ = random() * 3;
		timeToUpdateAngleDirection_ = random() * 1;
		targetAngle_ = random() * PI * 2;
		forward_ = true;
		backward_ = false;
		target_ = nullptr;
	}

	SurvivorBehavior::~SurvivorBehavior() {
	}

	void SurvivorBehavior::calculateInput(const Unit* unit, double time) {
		Input input;
		// extract information from memory
		// analyze situation - enviroment and units when neccesarry
		// make decision on what activity to perform
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
