#include "unitanimation.h"
#include "typedefs.h"
#include "auxiliary.h"
#include "unit.h"

namespace zombie {

	UnitAnimation::UnitAnimation(State state, float radius, const Animation& animation) : animation_(animation) {
		remove_ = false;
		state_ = state;
		radius_ = radius;
	}

	void UnitAnimation::updateData(Unit* unit, Unit::UnitEvent unitEvent) {
		state_ = unit->getState();
		radius_ = unit->getRadius();

		eventHandler(unitEvent);
	}

	bool UnitAnimation::update(float time) {
		// Draw body
		glPushMatrix();
		glTranslate2f(state_.position_);
		glScale2f(radius_);
		glRotated(state_.angle_ * 180 / PI, 0, 0, 1);
		animation_.draw(time);
		glPopMatrix();

		return !remove_;
	}

	void UnitAnimation::eventHandler(Unit::UnitEvent unitEvent) {
		switch (unitEvent) {
			case Unit::RUN:
				// Todo!
				break;
			case Unit::STANDSTILL:
				animation_.restart();
				break;
			case Unit::REMOVED:
				remove_ = true;
				break;
			default:
				break;
		}
	}

} // Namespace zombie.
