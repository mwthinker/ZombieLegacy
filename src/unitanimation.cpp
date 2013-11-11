#include "unitanimation.h"
#include "typedefs.h"
#include "auxiliary.h"
#include "unit.h"

namespace zombie {

	UnitAnimation::UnitAnimation(Unit* unit, const Animation& animation) : animation_(animation) {
		unit_ = unit;
		unit_->addEventHandler(std::bind(&UnitAnimation::eventHandler, this, std::placeholders::_1));
	}

	UnitAnimation::~UnitAnimation() {
	}

	// private
	void UnitAnimation::draw(float time, float timeStep, float accumulator) {
		Position p = unit_->getPosition();

		// Draw body
		glPushMatrix();
		glTranslated(p.x, p.y, 0);
		glScaled(unit_->getRadius()*0.9, unit_->getRadius()*0.9, 1);
		glRotated(unit_->getState().angle_ * 180 / PI, 0, 0, 1);
		animation_.draw(time);
		glPopMatrix();
	}

	void UnitAnimation::eventHandler(Unit::UnitEvent unitEvent) {
		switch (unitEvent) {
			case Unit::RUN:
				// Todo!
				break;
			case Unit::STANDSTILL:
				animation_.restart();
				break;
			default:
				break;
		}
	}

} // Namespace zombie.
