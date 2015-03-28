#include "survivorbehavior.h"
#include "unit.h"
#include "auxiliary.h"

#include <cassert>

namespace zombie {

	SurvivorBehavior::SurvivorBehavior(Unit* unit) {
		findNewTargetTime_ = random() * 3;
		target_ = nullptr;
		unit_ = unit;
		assert(unit_); // Null not allowed.
	}

	SurvivorBehavior::~SurvivorBehavior() {
	}

	void SurvivorBehavior::updateInput(double time, double deltaTime) {
		Input input;
		// extract information from memory
		// analyze situation - enviroment and units when neccesarry
		// make decision on what activity to perform
		//unit_->updatePhysics(time, deltaTime, input);
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

	MovingObject* SurvivorBehavior::getMovingObject() const {
		return unit_;
	}

} // Namespace zombie.
